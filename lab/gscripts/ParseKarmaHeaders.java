//Parse metoolkit's public headers and apply the real prototypes to imported functions.
//@category Karma
//
// Without this, Ghidra has no signature for an imported function and guesses its
// arity from the call site — badly. In IxBoxTriList it decided
// McdModelGetGeometry takes NO arguments, emitting the pushed arguments as
// writes to unrelated stack variables:
//
//     pMStack_25c = p->model1;
//     fStack_260  = 9.18817e-41;          // a pointer misread as a denormal float
//     boxgeom = (McdBoxID)McdModelGetGeometry();
//
// which is not a cosmetic problem — the recovered code would call it with
// garbage. Enabling "Decompiler Parameter ID" does not fix it; only real
// prototypes do.
import ghidra.app.script.GhidraScript;
import ghidra.app.cmd.function.ApplyFunctionSignatureCmd;
import ghidra.app.util.cparser.C.CParserUtils;
import ghidra.program.model.data.*;
import ghidra.program.model.listing.*;
import ghidra.program.model.symbol.SourceType;

public class ParseKarmaHeaders extends GhidraScript {
    @Override
    public void run() throws Exception {
        // Analysis has NOT run yet — headless order is import, preScript,
        // analysis, postScript — so this is the one place an analyzer option
        // can still be changed. Unset means "do not touch anything", which is
        // what produced out9.
        applyAnalysisOptions();

        // A flat, dependency-free prototype header produced by
        // tools/gen_protos.py from the same DWARF. metoolkit's real headers
        // do NOT survive Ghidra's C parser: they are layered with
        // MEAPI/MEPUBLIC macros and platform blocks it rejects.
        String hdr = System.getenv("KARMA_PROTOS");
        if (hdr == null) {
            println("KARMAHDR: KARMA_PROTOS not set, skipping");
            return;
        }
        DataTypeManager dtm = currentProgram.getDataTypeManager();
        String[] files = { hdr };
        String[] paths = new String[0];
        String[] args  = new String[0];

        try {
            CParserUtils.parseHeaderFiles(new DataTypeManager[] { dtm },
                                          files, paths, args, dtm, monitor);
        } catch (Exception e) {
            println("KARMAHDR: parse reported: " + e.getMessage());
            // Parsing is best-effort: metoolkit headers contain C++ and
            // platform blocks the C parser trips over, but everything it DID
            // manage to parse is still in the DTM and worth applying.
        }

        int nfd = 0;
        java.util.Iterator<DataType> all = dtm.getAllDataTypes();
        while (all.hasNext()) if (all.next() instanceof FunctionDefinition) nfd++;
        println("KARMAHDR: parsed FunctionDefinitions=" + nfd);

        // In a relocatable .o, an imported function is just an undefined symbol:
        // Ghidra creates no Function object for it, so getFunctions() and
        // getExternalFunctions() both miss it and the decompiler is left
        // guessing the arity at every call site. Walk the symbol table instead,
        // materialise a function at each such symbol, and apply the signature.
        ghidra.program.model.symbol.SymbolTable st = currentProgram.getSymbolTable();
        ghidra.program.model.listing.FunctionManager fm = currentProgram.getFunctionManager();
        int defined = 0, imported = 0, failed = 0;

        for (ghidra.program.model.symbol.Symbol sym : st.getAllSymbols(true)) {
            DataType dt = findDef(dtm, sym.getName());
            if (!(dt instanceof FunctionDefinition)) continue;
            ghidra.program.model.address.Address addr = sym.getAddress();
            if (addr == null) continue;
            Function f = fm.getFunctionAt(addr);
            boolean isNew = false;
            if (f == null) {
                try {
                    f = createFunction(addr, sym.getName());
                    isNew = f != null;
                } catch (Exception e) { /* not every symbol is code */ }
            }
            if (f == null) continue;
            ApplyFunctionSignatureCmd cmd = new ApplyFunctionSignatureCmd(
                addr, (FunctionDefinition) dt, SourceType.IMPORTED);
            if (cmd.applyTo(currentProgram, monitor)) {
                if (isNew) imported++; else defined++;
            } else failed++;
        }
        println("KARMAHDR: signatures applied -- defined=" + defined
                + " imported=" + imported + " failed=" + failed);
        dumpSignatures();
    }

    /**
     * Print the applied signature and per-parameter STORAGE for every function
     * whose name contains KD_DUMP_SIGS. A prototype that reads correctly in
     * kd_protos.h can still be laid out wrongly on the stack, and the layout is
     * what the decompiler uses — so print the storage, not the text.
     */
    private void dumpSignatures() {
        String want = System.getenv("KD_DUMP_SIGS");
        if (want == null || want.isEmpty()) return;
        for (Function f : currentProgram.getFunctionManager().getFunctions(true)) {
            if (!f.getName().contains(want)) continue;
            StringBuilder sb = new StringBuilder();
            sb.append("SIG: ").append(f.getName())
              .append(" cc=").append(f.getCallingConventionName())
              .append(" custom=").append(f.hasCustomVariableStorage())
              .append(" params=").append(f.getParameterCount()).append(" [");
            for (ghidra.program.model.listing.Parameter p : f.getParameters())
                sb.append(p.getName()).append(':').append(p.getDataType().getName())
                  .append('@').append(p.getVariableStorage()).append(' ');
            println(sb.append(']').toString());
        }
    }

    private DataType findDef(DataTypeManager dtm, String name) {
        java.util.List<DataType> hits = new java.util.ArrayList<>();
        dtm.findDataTypes(name, hits);
        for (DataType d : hits) if (d instanceof FunctionDefinition) return d;
        return null;
    }

    /**
     * Set analyzer options from KD_GHIDRA_OPTS, a ';'-separated list of
     * `Analyzer.Option Name=value` pairs, e.g.
     *
     *   KD_GHIDRA_OPTS='DWARF.Import Local Variable Info=false'
     *
     * Why this exists. For a handful of functions — MdtKeaAddConstraintForces
     * and friends — the DWARF declares parameters and locals with names and
     * types and NO DW_AT_location, because the abbrevs those DIEs use do not
     * carry one. Ghidra's DWARF importer creates the variables anyway, so the
     * decompiler is handed typed locals it cannot place and has to reconcile
     * them with its own stack analysis. The hypothesis this switch exists to
     * test is that the DWARF is making those frames WORSE, not better, and
     * that Ghidra's native stack recovery alone would do better.
     *
     * Unset changes nothing, which is what produced out9. The option names are
     * exactly the strings the analyzer registers (see DWARFImportOptions), and
     * an unknown name is reported rather than ignored — a silently-declined
     * option would look exactly like "the experiment showed no difference".
     */
    private void applyAnalysisOptions() {
        String spec = System.getenv("KD_GHIDRA_OPTS");
        if (spec == null || spec.trim().isEmpty()) {
            println("KARMAOPT: KD_GHIDRA_OPTS not set, analyzer options untouched");
            return;
        }
        ghidra.framework.options.Options opts =
            currentProgram.getOptions(Program.ANALYSIS_PROPERTIES);
        for (String kv : spec.split(";")) {
            kv = kv.trim();
            if (kv.isEmpty()) continue;
            int eq = kv.indexOf('=');
            if (eq < 0) { println("KARMAOPT: malformed '" + kv + "'"); continue; }
            String name = kv.substring(0, eq).trim();
            String val  = kv.substring(eq + 1).trim();
            if (!opts.contains(name)) {
                // Loudly, because the whole point of the experiment is to tell
                // "the option did nothing" from "the option was never applied".
                println("KARMAOPT: NO SUCH OPTION '" + name + "' -- NOT APPLIED");
                continue;
            }
            String before = opts.getValueAsString(name);
            setAnalysisOption(currentProgram, name, val);
            println("KARMAOPT: " + name + " " + before + " -> "
                    + opts.getValueAsString(name));
        }
    }
}
