import java.lang.*;

public class AutoexecGenerator implements Generator {
    private class AutoexecCommand extends TemplateCommand {
		private static final String WHITE_BALANCE = "WhiteBalance";
		private static final String GAIN = "Gain";
		private static final String SHUTTER_SPEED = "ShutterSpeed";
		private static final String GCD_THRESHOLD = "GCDThreshold";
		private static final String AIBO_NAME = "Name";
		private static final String AIBO_ID = "ID";
		private static final String ROLE = "ROLE";
		private static final String TARGET_GOAL = "TargetGoal";
		private static final String MESSAGE_FLAMESPAN = "MessageFlameSpan";
		private static final String MONITOR = "Monitor";
		private static final String GCDMONITOR = "GCDMonitor";
		private static final String PHOTOINTERVAL = "PhotoInterval";
		private static final String DEBUG_LOG = "DebugLog";
		private static final String INFO_LOG = "InfoLog";
		private static final String INFO_TARGET = "InfoTarget";
		private static final String DEBUG_TARGET = "DebugTarget";
		private static final String INFO_VCBEACONDISTCALIB = "VCBeaconDistCalib";
		private static final String INFO_VCGOALDISTCALIB1 = "VCGoalDistCalib1";
		private static final String INFO_VCGOALDISTCALIB2 = "VCGoalDistCalib2";
		private static final String INFO_VCBALLDISTCALIB1 = "VCBallDistCalib1";
		private static final String INFO_VCBALLDISTCALIB2 = "VCBallDistCalib2";
		private static final String INFO_VCTILTOFFSET = "VCTiltOffsetCalib";
		private static final String PWALKBODYTILTOFFSET = "PWalkGetBodyTiltOffset";

		public AutoexecCommand() {
			Class klass = AutoexecCommand.class;

			registerCommand(klass, WHITE_BALANCE, "replaceSimple");
			registerCommand(klass, GAIN,          "replaceSimple");
			registerCommand(klass, SHUTTER_SPEED, "replaceSimple");
			registerCommand(klass, GCD_THRESHOLD, "replaceSimple");
			registerCommand(klass, MESSAGE_FLAMESPAN, "replaceSimple");
			registerCommand(klass, AIBO_NAME);
			registerCommand(klass, AIBO_ID);
			registerCommand(klass, ROLE);
			registerCommand(klass, TARGET_GOAL);
			registerCommand(klass, MONITOR,       "replaceBoolean");
			registerCommand(klass, GCDMONITOR,    "replaceBoolean");
			registerCommand(klass, PHOTOINTERVAL, "replaceSimple");
			registerCommand(klass, DEBUG_LOG,     "replaceBoolean");
			registerCommand(klass, INFO_LOG,      "replaceBoolean");
			registerCommand(klass, INFO_TARGET,   "replaceSimple");
			registerCommand(klass, DEBUG_TARGET,   "replaceSimple");
			registerCommand(klass, INFO_VCBEACONDISTCALIB, "replaceSimple");
			registerCommand(klass, INFO_VCGOALDISTCALIB1, "replaceSimple");
			registerCommand(klass, INFO_VCGOALDISTCALIB2, "replaceSimple");
			registerCommand(klass, INFO_VCBALLDISTCALIB1, "replaceSimple");
			registerCommand(klass, INFO_VCBALLDISTCALIB2, "replaceSimple");
			registerCommand(klass, INFO_VCTILTOFFSET, "replaceSimple");
			registerCommand(klass, PWALKBODYTILTOFFSET, "replaceSimple");
		}

		public String replaceName(String cmd, Configuration config) {
			String role = config.get("ROLE");
			return "aibo" + config.get(role);
		}

		public String replaceID(String cmd, Configuration config) {
			String role = config.get("ROLE");
			return "" + (Integer.parseInt(config.get(role)) % 4);
		}

		public String replaceROLE(String cmd, Configuration config) {
			String role = config.get("ROLE");
			System.out.println("role = "+ role);

			if(role.equalsIgnoreCase("striker")) {
				return "STRIKER";
			}
			if(role.equalsIgnoreCase("libero")) {
				return "LIBERO";
			}
			if(role.equalsIgnoreCase("defender")) {
				return "DEFENDER";
			}
			if(role.equalsIgnoreCase("goalie")) {
				return "GOALIE";
			}

			Utility.fatalError("役割の指定が変です. " + role);
			return null;
		}

		public String replaceTargetGoal(String cmd, Configuration config) {
			String color = config.get("COLOR");
			if(color.equalsIgnoreCase("blue")) 
				return "YELLOW";
			else if(color.equalsIgnoreCase("red"))
				return "BLUE";

			Utility.fatalError("チームカラーの指定が変です。" + color);
			return null;
		}
    }

    private Configuration fConfig;
    private String fTemplate;
    private TemplateGenerator fGenerator;
    
    public AutoexecGenerator(Configuration config, String template) {
        fConfig = config;
        fTemplate = template;
		fGenerator = new TemplateGenerator(new AutoexecCommand());
    }
    
    public void generate(String output) {
		if(fTemplate.equals(output)) {
			Utility.fatalError("テンプレート("+fTemplate+
							   ")と出力ファイル("+output+
							   ")に同じ名前が指定されています。");
		}

		fGenerator.generate(fConfig, fTemplate, output);
    }

    public static void main(String params[]) {
        Configuration config = new Configuration("sample.cnf");
		config.put("ROLE", "Striker");

		AutoexecGenerator gen = new AutoexecGenerator(config, "AUTOEXEC.SCM.templ");
		gen.generate("gaga");
    }
}
