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
    }

    private DataType findDef(DataTypeManager dtm, String name) {
        java.util.List<DataType> hits = new java.util.ArrayList<>();
        dtm.findDataTypes(name, hits);
        for (DataType d : hits) if (d instanceof FunctionDefinition) return d;
        return null;
    }
}
