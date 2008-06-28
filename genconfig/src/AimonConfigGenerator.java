import java.lang.*;

public class AimonConfigGenerator implements Generator {
	public class AimonConfigCommand extends NetworkCommand {
		public static final String IP_ADDR="IP";
		public static final String NETWORK="Network";

		public AimonConfigCommand() {
			Class klass = AimonConfigCommand.class;
			registerCommand(klass, IP_ADDR);
		}

		public String replaceIP(String cmd, Configuration config) {
			String role = config.get("ROLE");
			String ip = getIPAddress(config, role);
			return ip;
		}
	}

	private Configuration fConfig;
	private String fTemplate;
	private TemplateGenerator fGenerator;

	public AimonConfigGenerator(Configuration config, String template) {
		fConfig = config;
		fTemplate = template;
		fGenerator = new TemplateGenerator(new AimonConfigCommand());
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
