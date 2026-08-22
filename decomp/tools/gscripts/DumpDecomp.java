//Decompile every function to C + emit per-function quality stats.
//@category Karma
import ghidra.app.script.GhidraScript;
import ghidra.app.decompiler.*;
import ghidra.program.model.listing.*;
import java.io.*;

public class DumpDecomp extends GhidraScript {
    @Override
    public void run() throws Exception {
        String outDir = System.getenv("KARMA_OUTDIR");
        if (outDir == null) outDir = "/tmp";
        String base = currentProgram.getName().replaceAll("[^A-Za-z0-9_.-]","_");
        DecompInterface di = new DecompInterface();
        di.setOptions(new DecompileOptions());
        di.openProgram(currentProgram);
        PrintWriter pw  = new PrintWriter(new FileWriter(outDir + "/" + base + ".c"));
        PrintWriter csv = new PrintWriter(new FileWriter(outDir + "/stats.csv", true));
        FunctionIterator it = currentProgram.getFunctionManager().getFunctions(true);
        while (it.hasNext()) {
            Function f = it.next();
            if (f.isExternal() || f.getBody().getNumAddresses() <= 8) continue; // skip PLT-ish stubs
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
    int countWarn(String c){ int n=0,i=0; while((i=c.indexOf("/* WARNING",i))>=0){n++;i+=5;} return n; }
}
