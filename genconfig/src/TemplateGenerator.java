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
			Utility.fatalError("�ƥ�ץ졼��("+template+")�����Ĥ���ޤ���");
		} catch (SecurityException e) {
			Utility.fatalError("�ƥ�ץ졼��("+template+")�������ޤ���");
		} catch (UnsupportedEncodingException e) {
			Utility.fatalError("encoding(JISAutoDetect) �򥵥ݡ��Ȥ��Ƥ��ޤ���");
		}
			
		try {
			FileOutputStream fos = new FileOutputStream(output);
			writer = new PrintWriter(new OutputStreamWriter(fos));
		} catch (FileNotFoundException e) {
			Utility.fatalError("���ϥե�����("+output+")�������Ǥ��ޤ���");
		} catch (SecurityException e) {
			Utility.fatalError("���ϥե�����("+output+")�������Ǥ��ޤ���");
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
						// �ѥ�����˥ޥå������Τ�¸�ߤ���
						before = m.group(1);
						command = m.group(2);
						after = m.group(3);

						String val = null;
						try {
							val = fCommand.replace(command, config);
						} catch (ReplacerNotFoundException e) {
							String msg = 
								"�ƥ�ץ졼�� ("+template+")��" +
								lineCounter + "���ܤˤ��� " + e.getCommand() +
								" ���Ѵ��Ǥ��ޤ���\n" +
								"�֤���ǧ���Ƥ���������";
							Utility.fatalError(msg);
						}
						
						writer.print(before + val);
						// �Ĥ����ʬ�� line �ˤ��ơ��⤦�����ִ���Ԥ�
						line = after;
						
						// �Ĥ����ʬ���ʤ���硢�롼�פ���λ����Τǲ���
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
