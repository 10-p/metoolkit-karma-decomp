//Decompile every function to C + emit per-function quality stats.
//@category Karma
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.data.*;
import ghidra.program.model.pcode.*;
import ghidra.program.model.address.Address;
import java.io.*;
import java.util.*;

public class DumpDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String outDir = System.getenv("KARMA_OUTDIR");
        if (outDir == null) outDir = "/tmp";
        String base = currentProgram.getName().replaceAll("[^A-Za-z0-9_.-]","_");
        // --- call-site signature overrides -------------------------------
        // Ghidra has no signature for a call through a function pointer, so it
        // emits `(*fn)()` with EVERY argument dropped and the callee reads the
        // stack. That is not cosmetic: it crashed IxSphereTriList in a live
        // match, because Karma calls back into the engine's KTriListGenerator
        // through McdTriangleListFnPtr and got garbage for `pos`.
        //
        // writeOverride() is Ghidra's own mechanism for this ("Override
        // Signature" in the UI). Applied at the call site, the next decompile
        // passes the arguments properly.
        applyCallsiteOverrides();

        // The same repair, aimed at C++ virtual calls instead of a C callback.
        // Ghidra drops every argument at a call through a function pointer, and
        // in the libMdtKea solver driver that is the whole blocker: the calls
        // into the three kernels already proven bit-identical come out as
        // `(**(code **)(_vanillaFunctions + 0x10))()`. Which function each slot
        // reaches is worked out by tools/gen_vtable_callsites.py, from
        // relocation records rather than from decompiled text, and handed here
        // as a table of addresses. See that file's docstring for why each row
        // is allowed to be believed.
        applyVtableCallsiteOverrides();

        // Ghidra decides a calling convention per function during analysis, and
        // for this corpus it decides wrong: 19 functions across 9 objects come
        // out tagged __regparm1 or __regparm2. gcc 3.2 on i386 passes everything
        // on the stack here — the prologues say so, and the DWARF says so.
        //
        // The tag is not the damage. Ghidra LAYS THE PARAMETER LIST OUT to match
        // the convention it chose, so in a __regparmN function every parameter
        // in the body is shifted by N and the last incoming argument falls off
        // the end. McdGeometryGetMassProperties passed three of its four
        // arguments, each one position off, and segfaulted in a live match.
        //
        // Forcing __cdecl before decompiling fixes it at the source rather than
        // downstream, where the shift is not reliably repairable: renaming the
        // parameters recovers the ones Ghidra did model, and the argument it
        // dropped at an inner call site is simply gone.
        forceCdecl();

        DecompInterface di = new DecompInterface();
        di.setOptions(new DecompileOptions());
        di.openProgram(currentProgram);
        PrintWriter pw  = new PrintWriter(new FileWriter(outDir + "/" + base + ".c"));
        PrintWriter csv = new PrintWriter(new FileWriter(outDir + "/stats.csv", true));
        // Every decompiled local, with the STACK OFFSET Ghidra assigned it.
        //
        // Ghidra prints a slot's ADDRESS as `&stack0xffffff6c` and does not
        // declare it, while the local that occupies that offset is printed
        // under whatever name the decompiler invented — `kVar2`, not
        // `in_stack_ffffff6c`. So the two can only be connected through the
        // offset, and the offset is in the symbol map, not in the text.
        //
        // Reading it out of the text instead would mean inferring the base of
        // `kVar2` from a line like `kVar2._kd[4] = in_stack_ffffff70[0]`, which
        // is precisely the guess the guessed-stack-frame detector exists to
        // stop. This is Ghidra's own answer.
        PrintWriter loc = new PrintWriter(new FileWriter(outDir + "/" + base + ".locals"));
        loc.println("# function,name,stackoff,hexoff,size,type");

        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            // Skip only Ghidra's synthetic import placeholders, NOT small real
            // functions. An earlier size test (<= 8 addresses) also dropped
            // genuine tiny functions, which then showed up downstream as
            // "undeclared identifier" errors (e.g. _McdGeometryDeinit).
            if (f.isExternal() || f.isThunk()) continue;
            ghidra.program.model.mem.MemoryBlock blk =
                currentProgram.getMemory().getBlock(f.getEntryPoint());
            if (blk == null || !blk.isInitialized()
                || blk.getName().toUpperCase().contains("EXTERNAL")) continue;
            DecompileResults r = di.decompileFunction(f, 180, monitor);
            boolean ok = r != null && r.decompileCompleted() && r.getDecompiledFunction()!=null;
            String c = ok ? r.getDecompiledFunction().getC() : "";
            if (ok) { pw.println("/* ==== " + f.getName() + " ==== */"); pw.println(c); }
            if (ok && r.getHighFunction() != null) dumpLocals(loc, f, r);
            int lines = c.isEmpty()?0:c.split("\n").length;
            csv.printf("%s,%s,%d,%d,%b,%b,%b,%b,%b,%b,%d%n",
                base, f.getName(), f.getBody().getNumAddresses(), lines, ok,
                c.contains("in_ST") || c.contains("unaff_FP"),   // x87 stack leak
                c.contains("halt_baddata"),                        // bad decode
                c.contains("UNRECOVERED"),                         // unresolved jump
                c.contains("Unresolved local var"),                // DWARF local not mapped
                c.contains("undefined") || c.contains("unaff_"),   // unknown types/regs
                countWarn(c));
        }
        pw.close(); csv.close(); loc.close();
        println("DONE " + base);
    }

    /**
     * One row per decompiled local that lives on the stack.
     *
     * The offset is the one Ghidra assigns, printed both signed and as the
     * 8-hex-digit two's complement form it uses when it builds a name — so
     * `stack0xffffff6c` and `in_stack_ffffff6c` both key straight into this
     * table, and so does a local whose name contains no offset at all.
     *
     * Locals in registers are skipped: they have no frame offset, and a row
     * without one would invite exactly the guess this file exists to avoid.
     */
    private void dumpLocals(PrintWriter loc, Function f, DecompileResults r) {
        try {
            java.util.Iterator<HighSymbol> it =
                r.getHighFunction().getLocalSymbolMap().getSymbols();
            while (it.hasNext()) {
                HighSymbol hs = it.next();
                ghidra.program.model.listing.VariableStorage st = hs.getStorage();
                if (st == null || !st.isStackStorage()) continue;
                int off = st.getStackOffset();
                DataType dt = hs.getDataType();
                int size = dt != null ? dt.getLength() : st.size();
                loc.printf("%s,%s,%d,%08x,%d,%s%n",
                           f.getName(), hs.getName(), off, off & 0xFFFFFFFFL, size,
                           dt != null ? dt.getName().replace(',', ';') : "?");
            }
        } catch (Exception e) {
            println("LOCALS: " + f.getName() + ": " + e.getMessage());
        }
    }
    /** int (McdModelPair*, McdUserTriangle*, MeVector3, MeReal, int) */
    /** Make every function plain cdecl, and say how many had to be changed. */
    private void forceCdecl() {
        int changed = 0, failed = 0;
        // Report the distribution before touching anything. HANDOVER.md 5 records
        // that forcing __cdecl on EVERYTHING made things worse and cost five kea
        // objects, so any widening of this rule has to be aimed at a measured
        // population rather than applied blind.
        java.util.Map<String,Integer> hist = new java.util.TreeMap<>();
        FunctionIterator ith = currentProgram.getFunctionManager().getFunctions(true);
        while (ith.hasNext()) {
            String c = ith.next().getCallingConventionName();
            hist.merge(c == null ? "(null)" : c, 1, Integer::sum);
        }
        println("KARMAHDR: calling conventions " + hist);

        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            String cc = f.getCallingConventionName();
            // ONLY the misdetection. A first attempt forced __cdecl on
            // everything and made things worse: gcc's i386 C++ ABI passes
            // `this` as the first stack argument, so __thiscall is already
            // right, and overriding it cost five kea objects that had been
            // compiling. __regparm is the one Ghidra gets wrong here — the
            // prologues read arguments from ebp+8 upwards, and the DWARF
            // agrees.
            // Two populations, and the distinction is load-bearing.
            //
            // __regparmN is a MISDETECTION: gcc 3.2 on i386 passes everything on
            // the stack here and Ghidra lays the parameter list out to match the
            // convention it chose, shifting the body by N.
            //
            // "unknown" is different — Ghidra never decided at all, and the
            // decompiler then emits `WARNING: Unknown calling convention` and
            // falls back to guessing the frame. keaRbdCore_unified, the libMdtKea
            // driver, has ALL ELEVEN of its functions in this state, which is why
            // its body is a wall of in_stack_fffffeNN and register0x00000010.
            // keaMemory has 7 of 11, keaIntegrate_pc 1 of 3 — the three objects
            // blocking the solver.
            //
            // ON BY DEFAULT, and measured before being made so. It is narrower
            // than the blanket force HANDOVER.md 5 records as harmful — that one
            // also hit __thiscall, and gcc's i386 C++ ABI passes `this` as the
            // first stack argument, so Ghidra's __thiscall is already right and
            // overriding it dropped the parameter, costing five kea objects.
            // Leaving __thiscall and __cdecl alone is the whole point.
            //
            // With the by-value aggregate prototypes (gen_protos.simple_type) it
            // is not merely cosmetic: without it, keaCalcJinvMandRHS_vanilla and
            // keaRbdCore_unified stop compiling. Corpus-wide it takes decompiler
            // warnings from 602 to 265 and changes nothing else the gates measure.
            // Set KD_FORCE_CDECL_UNKNOWN=0 to turn it off for an A/B.
            boolean unknown = (cc == null || cc.equals("unknown"))
                              && !"0".equals(System.getenv("KD_FORCE_CDECL_UNKNOWN"));
            if (!unknown && (cc == null || !cc.startsWith("__regparm"))) continue;
            try {
                // Custom storage pins parameters to the locations the wrong
                // convention chose, so it has to go first or the change is
                // cosmetic.
                f.setCustomVariableStorage(false);
                f.setCallingConvention("__cdecl");
                changed++;
            } catch (Exception e) { failed++; }
        }
        println("KARMAHDR: __regparm -> __cdecl on " + changed + " function(s), "
                + failed + " refused");
    }

    /**
     * Apply a real signature at each C++ virtual call site named in
     * KARMA_VTABLE_CALLSITES.
     *
     * The rows are `<object> <ghidra-address> <method> <class>+<slot>`, produced
     * by tools/gen_vtable_callsites.py by reading the vptr store's relocation
     * ("vtable for keaFunctions_Vanilla", addend +8 = the Itanium ABI address
     * point) and the vtable's own relocation records. Nothing in them is
     * inferred from decompiled output.
     *
     * The signature itself is whatever ParseKarmaHeaders already parsed out of
     * kd_protos.h into this program's DataTypeManager, looked up by the bare
     * method name — the same source, and the same `this`-as-first-stack-argument
     * convention, that every other call in the corpus uses.
     *
     * Applying a WRONG signature here would be worse than applying none: it
     * would call the right function with the wrong arguments and still compile.
     * So a row is skipped, loudly, if the address is not inside a function, if
     * the method has no prototype, or if the override is refused.
     */
    private void applyVtableCallsiteOverrides() {
        String table = System.getenv("KARMA_VTABLE_CALLSITES");
        if (table == null) {
            println("VTABLE: KARMA_VTABLE_CALLSITES not set, skipping");
            return;
        }
        String me = currentProgram.getName();
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        FunctionManager fm = currentProgram.getFunctionManager();
        int applied = 0, noproto = 0, nofunc = 0, refused = 0, rows = 0;

        try (java.io.BufferedReader r =
                 new java.io.BufferedReader(new FileReader(table))) {
            String line;
            while ((line = r.readLine()) != null) {
                line = line.trim();
                if (line.isEmpty() || line.startsWith("#")) continue;
                String[] p = line.split("\\s+");
                if (p.length < 3) continue;
                // The table covers the whole corpus; each program takes its own.
                if (!p[0].equals(me)) continue;
                rows++;
                Address site = currentProgram.getAddressFactory()
                                   .getDefaultAddressSpace()
                                   .getAddress(Long.parseLong(p[1].substring(2), 16));
                Function host = fm.getFunctionContaining(site);
                if (host == null) { nofunc++; continue; }

                DataType dt = null;
                java.util.List<DataType> hits = new ArrayList<>();
                dtm.findDataTypes(p[2], hits);
                for (DataType d : hits)
                    if (d instanceof FunctionDefinition) { dt = d; break; }
                if (dt == null) { noproto++;
                    println("VTABLE: no prototype for " + p[2] + " at " + p[1]);
                    continue; }

                try {
                    HighFunctionDBUtil.writeOverride(
                        host, site, (FunctionDefinition) dt);
                    applied++;
                } catch (Exception e) {
                    refused++;
                    println("VTABLE: override refused at " + p[1] + ": " + e.getMessage());
                }
            }
        } catch (Exception e) {
            println("VTABLE: could not read " + table + ": " + e.getMessage());
            return;
        }
        println("VTABLE: " + me + " rows=" + rows + " applied=" + applied
                + " no-prototype=" + noproto + " not-in-a-function=" + nofunc
                + " refused=" + refused);
    }

    private FunctionDefinitionDataType triListFnSig() {
        FunctionDefinitionDataType sig = new FunctionDefinitionDataType("McdTriangleListFn");
        sig.setReturnType(IntegerDataType.dataType);
        DataType pv = new PointerDataType(VoidDataType.dataType);
        sig.setArguments(new ParameterDefinition[] {
            new ParameterDefinitionImpl("modelTriListPair", pv, null),
            new ParameterDefinitionImpl("triangle",         pv, null),
            new ParameterDefinitionImpl("pos",              pv, null),
            new ParameterDefinitionImpl("radius", FloatDataType.dataType, null),
            new ParameterDefinitionImpl("maxTriangles", IntegerDataType.dataType, null),
        });
        return sig;
    }

    private void applyCallsiteOverrides() throws Exception {
        // Only in TriangleList intersection functions. The signature applied
        // is McdTriangleListFnPtr, and that is the ONLY callback Karma invokes
        // from those routines — it asks the host for triangles near the query
        // point. Other objects have indirect calls too (McdInteractions calls
        // McdIntersectFn), and applying the wrong signature there would be worse
        // than applying none, so the rule is scoped by the containing function's
        // name rather than by object.

        DecompInterface di = new DecompInterface();
        di.setOptions(new DecompileOptions());
        di.openProgram(currentProgram);
        FunctionDefinitionDataType sig = triListFnSig();
        int n = 0;
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        List<Function> fns = new ArrayList<>();
        while (it.hasNext()) fns.add(it.next());
        for (Function f : fns) {
            if (!f.getName().contains("TriangleList")) continue;
            DecompileResults r = di.decompileFunction(f, 120, monitor);
            if (r == null || r.getHighFunction() == null) continue;
            Iterator<PcodeOpAST> ops = r.getHighFunction().getPcodeOps();
            while (ops.hasNext()) {
                PcodeOpAST op = ops.next();
                if (op.getOpcode() != PcodeOp.CALLIND) continue;
                Address site = op.getSeqnum().getTarget();
                try {
                    HighFunctionDBUtil.writeOverride(f, site, sig);
                    n++;
                } catch (Exception e) {
                    println("OVERRIDE failed at " + site + ": " + e.getMessage());
                }
            }
        }
        di.dispose();
        println("OVERRIDES applied: " + n);
    }

    int countWarn(String c){ int n=0,i=0; while((i=c.indexOf("/* WARNING",i))>=0){n++;i+=5;} return n; }
}
