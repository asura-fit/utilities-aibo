import java.lang.*;
import java.util.*;
import java.io.File;

/**
 * 実行時の引数の内容を保持する
 */
public class CommandParameter {
    public final static String TARGET_AUTOEXEC = "AUTOEXEC";
    public final static String TARGET_WLANCONF = "WLANCONF";
    public final static String TARGET_AIMON    = "AIMON";
    public final static String TARGET_HUB_HOSTGW = "HUB_HOSTGW";
    public final static String TARGET_HUB_CONNECT = "HUB_CONNECT";

    private String fTemplateFile;
    private String fOutputFile;
    private String fTarget;
    private String fConfigureFile;
    private String fColor;
    private String[] fRoles;

    public CommandParameter(String params[]) {
		fTemplateFile = null;
		fOutputFile = null;
		fTarget = null;
		fConfigureFile = null;
		fRoles = null;
		fColor = null;

		params = parseOptions(params);
		parseCommands(params);
    }

	public String getTarget() {
		return fTarget;
	}
	public String getTemplateFile() {
		return fTemplateFile;
	}
	public String getOutputFile() {
		return fOutputFile;
	}
	public String getConfigureFile() {
		return fConfigureFile;
	}
	public String getColor() {
		return fColor;
	}
	public String[] getRoles() {
		return fRoles;
	}

    private String[] parseOptions(String[] params) {
		Vector rest_args = new Vector();
	
		for(int i = 0; i < params.length; i++) {
			if(params[i].startsWith("-t")) {
				String value;
				if(params[i].length() == 2) {
					// 次の引数に値が入っている
					value = params[i + 1];
					i++;
				} else {
					// -t にくっついている
					value = params[i].substring(2);
				}
				fTemplateFile = value;
			} else if(params[i].startsWith("-o")) {
				String value;
				if(params[i].length() == 2) {
					value = params[i + 1];
					i++;
				} else {
					value = params[i].substring(2);
				}
				fOutputFile = value;
			} else {
				rest_args.add(params[i]);
			}
		}

		int len = rest_args.size();
		String[] ret = new String[len];
		int j = 0;
		for(Iterator i = rest_args.iterator(); i.hasNext();j++) {
			ret[j] = (String )i.next();
		}
		return ret;
    }

	private boolean checkColor(String color) {
		if(fColor.equalsIgnoreCase("red"))
			return true;
		if(fColor.equalsIgnoreCase("blue"))
			return true;
		return false;
	}
	
	private boolean checkRole(String role) {
		if(role.equalsIgnoreCase("striker"))
			return true;
		if(role.equalsIgnoreCase("libero"))
			return true;
		if(role.equalsIgnoreCase("defender"))
			return true;
		if(role.equalsIgnoreCase("goalie"))
			return true;
		return false;
	}

    private void parseCommands(String params[]) {
		if(params.length < 2) {
			usage();
			Utility.fatalError("オプションを確認してください");
		}

		fConfigureFile = params[0];
		// 設定ファイルが存在するかチェック
		try {
			File f = new File(fConfigureFile);
			if(!f.exists())
				Utility.fatalError("設定ファイル("+fConfigureFile+")が存在しません");
			if(!f.canRead())
				Utility.fatalError("設定ファイル("+fConfigureFile+")を読めません");
		} catch (Exception e) {
			e.printStackTrace();
			Utility.fatalError("例外が発生しました。");
		}

		fTarget = params[1];
		if(fTarget.equals(TARGET_AUTOEXEC)) {
			parseAutoexecCommand(params);
		} else if(fTarget.equals(TARGET_WLANCONF)) {
			parseWlanconfCommand(params);
		} else if(fTarget.equals(TARGET_AIMON)) {
			parseAimonCommand(params);
		} else if(fTarget.equals(TARGET_HUB_HOSTGW)) {
			parseHubHostgwCommand(params);
		} else if(fTarget.equals(TARGET_HUB_CONNECT)) {
			parseHubConnectCommand(params);
		} else {
			usage();
			Utility.fatalError("ターゲットの指定が違います。");
		}
    }

	private void parseAutoexecCommand(String params[]) {
		if(params.length != 4) {
			usageAUTOEXEC();
			Utility.fatalError("引数の数が違います");
		}

		fColor = params[2];
		if(!checkColor(fColor)) {
			usageAUTOEXEC();
			Utility.fatalError("チームカラーの指定が変です("+fColor+")");
		}
		
		fRoles = new String[1];
		fRoles[0] = params[3];
		if(!checkRole(fRoles[0])){
			usageAUTOEXEC();
			Utility.fatalError("役割の指定が変です("+fRoles[0]+")");
		}
	}

	private void parseWlanconfCommand(String params[]) {
		if(params.length != 3) {
			usageWLANCONF();
			Utility.fatalError("引数の数が違います");
		}

		fRoles = new String[1];
		fRoles[0] = params[2];
		if(!checkRole(fRoles[0])) {
			usageWLANCONF();
			Utility.fatalError("役割の指定が変です("+fRoles[0]+")");
		}
	}

	private void parseAimonCommand(String params[]) {
		if(params.length != 3) {
			usageAIMON();
			Utility.fatalError("引数の数が違います");
		}

		fRoles = new String[1];
		fRoles[0] = params[2];
		if(!checkRole(fRoles[0])) {
			usageAIMON();
			Utility.fatalError("役割の指定が変です("+fRoles[0]+")");
		}
	}

	private void parseHubHostgwCommand(String params[]) {
		int len = params.length - 2;
		if(len > 4 || len < 0) {
			usageHUB_HOSTGW();
			Utility.fatalError("引数の数が違います");
		}

		fRoles = new String[len];
		for(int i = 0; i < len; i++) {
			fRoles[i] = params[i + 2];
			if(!checkRole(fRoles[i])) {
				usageHUB_HOSTGW();
				Utility.fatalError("役割の指定が変です("+fRoles[i]+")");
			}
		}

		// 重複チェック
		for(int i = 0; i < len; i++) {
			for(int j = i + 1; j < len; j++) {
				if(fRoles[i].equalsIgnoreCase(fRoles[j])) {
					usageHUB_HOSTGW();
					Utility.fatalError("役割の指定が重複しています(" + fRoles[i]);
				}
			}
		}
	}

	private void parseHubConnectCommand(String params[]) {
		int len = params.length - 2;
		if(len > 4 || len < 0) {
			usageHUB_CONNECT();
			Utility.fatalError("引数の数が違います");
		}

		fRoles = new String[len];
		for(int i = 0; i < len; i++) {
			fRoles[i] = params[i + 2];
			if(!checkRole(fRoles[i])) {
				usageHUB_CONNECT();
				Utility.fatalError("役割の指定が変です("+fRoles[i]+")");
			}
		}

		// 重複チェック
		for(int i = 0; i < len; i++) {
			for(int j = i + 1; j < len; j++) {
				if(fRoles[i].equalsIgnoreCase(fRoles[j])) {
					usageHUB_CONNECT();
					Utility.fatalError("役割の指定が重複しています(" + fRoles[i]);
				}
			}
		}
	}

	public void usage() {
		String[] msgs = {
			"Usage: java -jar genconfig.jar [options...] 設定ファイル TARGET",
			"",
			"[TARGET]",
			"   AUTOEXEC   -- AUTOEXEC.SCM を生成",
			"   WLANCONF   -- WLANCONF.TXT を生成",
			"   AIMON      -- AiboProxy 用の HOSTGW.CFG を生成",
			"   HUB_CONNECT-- AiboHUB 用の CONNECT.CFG を生成",
			"   HUB_HOSTGW -- AiboHUB 用の HOSTGW.CFG を生成",
			"",
			"[option]",
			"   -t template - テンプレートとして用いるファイル名を指定",
			"   -o output   - 生成するファイル名",
		};
		Utility.usagePrinter(msgs);
	}

	public void usageAUTOEXEC() {
		String[] msgs = {
			"Usage: java -jar genconfig.jar [options...] 設定ファイル AUTOEXEC team-color role",
			"   team-color: Red, Blue のいずれか",
			"   role: Striker, Libero, Defender, Goalie のいずれか",
		};
		Utility.usagePrinter(msgs);
	}

	public void usageWLANCONF() {
		String[] msgs = {
			"Usage: java -jar genconfig.jar [options...] 設定ファイル WLANCONF role",
			"  role: Striker, Libero, Defender, Goalie のいずれか",
		};
		Utility.usagePrinter(msgs);
	}

	public void usageAIMON() {
		String[] msgs = {
			"Usage: java -jar genconfig.jar [options...] 設定ファイル AIMON role",
			"  role: Striker, Libero, Defender, Goalie のいずれか",
		};
		Utility.usagePrinter(msgs);
	}

	public void usageHUB_CONNECT() {
		String[] msgs = {
			"Usage: java -jar genconfig.jar [options...] 設定ファイル HUB_CONNECT roles...",
			"  role: Striker, Libero, Defender, Goalie",
			"  roles... の部分には接続する AIBO の role を空白で区切って並べます"
		};
		Utility.usagePrinter(msgs);
	}

	public void usageHUB_HOSTGW() {
		String[] msgs = {
			"Usage: java -jar genconfig.jar [options...] 設定ファイル HUB_HOSTGW roles...",
			"  role: Striker, Libero, Defender, Goalie",
			"  roles... の部分には接続する AIBO の role を空白で区切って並べます"
		};
		Utility.usagePrinter(msgs);
	}
	
	private static CommandParameter sInstance;
	public static void initialize(String[] params) {
		sInstance = new CommandParameter(params);
	}
	public static CommandParameter getInstance() {
		if(sInstance == null)
			Utility.fatalError("CommandParameter の初期化が行われていません");
		return sInstance;
	}
}
