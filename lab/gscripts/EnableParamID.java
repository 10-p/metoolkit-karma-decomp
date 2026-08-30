//Enable analyzers that recover call-site argument passing.
//@category Karma
import ghidra.app.script.GhidraScript;

public class EnableParamID extends GhidraScript {
    @Override
    public void run() throws Exception {
        setAnalysisOption(currentProgram, "Decompiler Parameter ID", "true");
        setAnalysisOption(currentProgram, "Decompiler Switch Analysis", "true");
        println("PARAMID: enabled");
    }
}
