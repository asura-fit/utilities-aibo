import java.lang.*;
import java.io.*;
import java.util.regex.*;

public class TemplateGenerator {
    private TemplateCommand fCommand;

    public TemplateGenerator(TemplateCommand cmd) {
		fCommand = cmd;
    }

    public void generate(Configuration config, String template, String output)
	{
		BufferedReader reader = null;
		PrintWriter writer = null;

		try {
			FileInputStream fis = new FileInputStream(template);
			reader = new BufferedReader(new InputStreamReader(fis, "JISAutoDetect"));
		} catch (FileNotFoundException e) {
			Utility.fatalError("テンプレート("+template+")が見つかりません");
		} catch (SecurityException e) {
			Utility.fatalError("テンプレート("+template+")が開けません");
		} catch (UnsupportedEncodingException e) {
			Utility.fatalError("encoding(JISAutoDetect) をサポートしていません");
		}
			
		try {
			FileOutputStream fos = new FileOutputStream(output);
			writer = new PrintWriter(new OutputStreamWriter(fos));
		} catch (FileNotFoundException e) {
			Utility.fatalError("出力ファイル("+output+")を生成できません");
		} catch (SecurityException e) {
			Utility.fatalError("出力ファイル("+output+")を生成できません");
		}

		Pattern pattern = Pattern.compile("^([^$]*)\\$\\{([^}]*)\\}(.*)$");
		String line, before, after, command;
		try {
			int lineCounter = 0;
			while((line = reader.readLine()) != null) {
				lineCounter++;
				do {
					before = null;
					after = null;
					command = null;
					
					Matcher m = pattern.matcher(line);
					if(m.matches()) {
						// パターンにマッチするものが存在する
						before = m.group(1);
						command = m.group(2);
						after = m.group(3);

						String val = null;
						try {
							val = fCommand.replace(command, config);
						} catch (ReplacerNotFoundException e) {
							String msg = 
								"テンプレート ("+template+")の" +
								lineCounter + "行目にある " + e.getCommand() +
								" を変換できません。\n" +
								"綴りを確認してください。";
							Utility.fatalError(msg);
						}
						
						writer.print(before + val);
						// 残りの部分を line にして、もう一度置換を行う
						line = after;
						
						// 残りの部分がない場合、ループが終了するので改行
						if(line == null)
							writer.println("");
					} else {
						writer.println(line);
					}
				} while(after != null);
			}
			writer.close();
			reader.close();
		} catch (IOException e) {
			e.printStackTrace();
			System.exit(-1);
		}
    }
}
