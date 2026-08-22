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

        DecompInterface di = new DecompInterface();
        di.setOptions(new DecompileOptions());
        di.openProgram(currentProgram);
        PrintWriter pw  = new PrintWriter(new FileWriter(outDir + "/" + base + ".c"));
        PrintWriter csv = new PrintWriter(new FileWriter(outDir + "/stats.csv", true));
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
        pw.close(); csv.close();
        println("DONE " + base);
    }
    /** int (McdModelPair*, McdUserTriangle*, MeVector3, MeReal, int) */
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
