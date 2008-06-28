import java.lang.*;

public class WLanConfigGenerator implements Generator {
	public class WLanConfigCommand extends NetworkCommand {
		public static final String IP_ADDR="IP";
		public static final String NETWORK="Network";
		public static final String NETMASK="NetMask";
		public static final String ESSID="ESSID";
		public static final String WEPENABLE="WEPENABLE";
		public static final String WEPKEY="WEPKEY";
		public static final String APMODE="APMODE";
		public static final String CHANNEL="CHANNEL";
		public static final String NAME="Name";

		public WLanConfigCommand() {
			Class klass = WLanConfigCommand.class;
			registerCommand(klass, NETWORK, "replaceSimple");
			registerCommand(klass, NETMASK, "replaceSimple");
			registerCommand(klass, ESSID,   "replaceSimple");
			registerCommand(klass, WEPENABLE, "replaceSimple");
			registerCommand(klass, WEPKEY, "replaceSimple");
			registerCommand(klass, APMODE, "replaceSimple");
			registerCommand(klass, CHANNEL, "replaceSimple");
			registerCommand(klass, IP_ADDR);
			registerCommand(klass, NAME);
		}

		public String replaceIP(String cmd, Configuration config) {
			String role = config.get("ROLE");
			String ip = getIPAddress(config, role);
			return ip;
		}

		public String replaceName(String cmd, Configuration config) {
			String role = config.get("ROLE");
			return "aibo" + config.get(role);
		}
	}

	private Configuration fConfig;
	private String fTemplate;
	private TemplateGenerator fGenerator;

	public WLanConfigGenerator(Configuration config, String template) {
		fConfig = config;
		fTemplate = template;
		fGenerator = new TemplateGenerator(new WLanConfigCommand());
	}

	public void generate(String output) {
		if(fTemplate.equals(output)) {
			Utility.fatalError("テンプレート("+fTemplate+
							   ")と出力ファイル("+output+
							   ")に同じ名前が指定されています。");
		}

		fGenerator.generate(fConfig, fTemplate, output);
	}
}
