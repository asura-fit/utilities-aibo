import java.lang.*;
import java.io.*;

public class ConfigurationGenerator {
    private static final String sTempleteDir = "template/";

    public static void main(String args[]) {
		// 引数の解析
		CommandParameter.initialize(args);
		CommandParameter param = CommandParameter.getInstance();

		Configuration config = new Configuration(param.getConfigureFile());
		String target = param.getTarget();
		Generator gen = null;
		String output = param.getOutputFile();
		String template = param.getTemplateFile();

		if(target.equals(CommandParameter.TARGET_AUTOEXEC)) {
			if(output == null)
				output = "AUTOEXEC.SCM";
			if(template == null)
				template = sTempleteDir + "AUTOEXEC.SCM.templ";
			config.put("ROLE",  param.getRoles()[0]);
			config.put("COLOR", param.getColor());
			
			gen = new AutoexecGenerator(config, template);
		} else if(target.equals(CommandParameter.TARGET_WLANCONF)) {
			if(output == null)
				output = "WLANCONF.TXT";
			if(template == null)
				template = sTempleteDir + "WLANCONF.TXT.templ";
			config.put("ROLE",  param.getRoles()[0]);

			gen = new WLanConfigGenerator(config, template);
		} else if(target.equals(CommandParameter.TARGET_AIMON)) {
			if(output == null)
				output = "HOSTGW.CFG";
			if(template == null)
				template = sTempleteDir + "AIMON.HOSTGW.CFG.templ";
			config.put("ROLE",  param.getRoles()[0]);

			gen = new AimonConfigGenerator(config, template);
		} else if(target.equals(CommandParameter.TARGET_HUB_HOSTGW)) {
			if(output == null)
				output = "HOSTGW.CFG";
			
			gen = new HubHostgwGenerator(config);
		} else if(target.equals(CommandParameter.TARGET_HUB_CONNECT)) {
			if(output == null)
				output = "CONNECT.CFG";
			
			gen = new HubConnectGenerator(config);
		}

		if(gen == null) {
			// でるはずのないエラー
			Utility.fatalError("generator が存在しません");
		}
				  
		gen.generate(output);
    }
}
 
