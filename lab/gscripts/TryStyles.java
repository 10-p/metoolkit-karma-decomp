//Decompile one function under several simplification styles and report which,
//if any, avoids the stack0xNNNN pseudo-symbols.
//@category Karma
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;

public class TryStyles extends GhidraScript {
    @Override
    public void run() throws Exception {
        String target = System.getenv("KD_FN");
        if (target == null) target = "McdSphereTriangleListIntersect";
        String[] styles = { "decompile", "normalize", "firstpass", "register", "paramid" };
        for (String style : styles) {
            DecompInterface di = new DecompInterface();
            DecompileOptions o = new DecompileOptions();
            o.setEliminateUnreachable(false);
            di.setOptions(o);
            di.openProgram(currentProgram);
            try { di.setSimplificationStyle(style); }
            catch (Exception e) { println("STYLE " + style + ": unsupported"); continue; }
            FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
            boolean found = false;
            while (it.hasNext()) {
                Function f = it.next();
                if (!f.getName().equals(target)) continue;
                found = true;
                DecompileResults r = di.decompileFunction(f, 180, monitor);
                if (r == null || !r.decompileCompleted() || r.getDecompiledFunction() == null) {
                    println("STYLE " + style + ": decompile FAILED");
                    break;
                }
                String c = r.getDecompiledFunction().getC();
                int nstack = c.split("stack0x", -1).length - 1;
                int nlines = c.split("\n").length;
                println("STYLE " + style + ": stack0x=" + nstack + " lines=" + nlines);
                break;
            }
            if (!found) println("STYLE " + style + ": function not present");
            di.dispose();
        }
    }
}
