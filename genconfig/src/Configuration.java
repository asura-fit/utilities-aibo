import java.lang.*;
import java.io.*;
import java.util.regex.*;
import java.util.*;

class ParsingException extends Exception {
	/** ���顼�����������ι��ֹ� */
	private int fLine;
	private String fMessage;
	
	ParsingException(int line, String msg) {
		super();
		fLine = line;
		fMessage = msg;
	}

	public int getLine() {
		return fLine;
	}
	public String getErrorMessage() {
		return fMessage;
	}
}

public class Configuration{
    private Map fValueMap;
    
    public Configuration(String configFile) {
        fValueMap = new HashMap();
        
        try {
            FileInputStream fis = new FileInputStream(configFile);
            BufferedReader reader;
            reader = new BufferedReader(new InputStreamReader(fis));
            
            String line;
			int lineCounter = 1;
            while((line = reader.readLine()) != null) {
                line = removeComment(line);
                if(isEmptyLine(line)) // ���Ԥ�̵�뤹��
                    continue;
                
				try {
					String[] vals = parseLine(line, lineCounter);
					put(vals[0], vals[1]);
				} catch (ParsingException e) {
					Utility.error("�� " + e.getLine() + ": " + e.getErrorMessage());
					Utility.error("���ιԤ�̵�뤷��³���ޤ�");
				}

				lineCounter++;
            }
        } catch (Exception e) {
			Utility.fatalError("����ե�����(" + configFile +
								")�����Ĥ���ޤ���");
        }
    }
    
    public void put(String label, String value) {
		fValueMap.put(label.toLowerCase(), value);
    }

    public String get(String label) {
        Object val = fValueMap.get(label.toLowerCase());
        if(val != null)
            return (String )val;

        return null;
    }
    
    private String removeComment(String line) {
        int index = line.indexOf('#');
        if(index == -1) {
            return line;
        }
        
        return line.substring(0, index);
    }
    
    private boolean isEmptyLine(String line) {
        if(line.length() == 0)
            return true;
        
        for(int i = 0; i < line.length(); i++) {
            if(!Character.isWhitespace(line.charAt(i)))
                return false;
        }
        return true;
    }
    
    private String[] parseLine(String line, int lineCounter)
		throws ParsingException {
        String vals[] = {null, null};
        
        int index = line.indexOf('=');
        if(index == -1) {
            throw new ParsingException(lineCounter,
									   "label = value���η����ǤϤ���ޤ���");
		}
        
        vals[0] = line.substring(0, index).trim();
        vals[1] = line.substring(index + 1).trim();
        
        return vals;
    }
}
