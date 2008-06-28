/*
 * TemplateCommand.java
 *
 * Created on 2003/04/22, 8:12
 */
import java.lang.*;
import java.lang.reflect.*;
import java.util.*;

class ReplacerNotFoundException extends Exception {
	private String fCommand;

	public ReplacerNotFoundException(String command) {
		fCommand = command;
	}

	public String getCommand() {
		return fCommand;
	}
}

/**
 *
 * @author  tak
 */
public class TemplateCommand {
    private Map fCommandMap;
     
    /** Creates a new instance of TemplateCommand */
    public TemplateCommand() {
		fCommandMap = new HashMap();
    }
    
    protected void registerCommand(Class klass, String cmd) {
		Class[] paramTypes = {String.class, Configuration.class};
		String methodName = "replace" + cmd;

		try {
			Method m = klass.getMethod(methodName, paramTypes);
			fCommandMap.put(cmd, m);
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
			Utility.fatalError("プログラムが変です");
		} catch (SecurityException e) {
			e.printStackTrace();
			Utility.fatalError("メソッド(" + methodName +")の情報を取得できません");
		}
    }

    protected void registerCommand(Class klass, String cmd, String methodName){
		Class[] paramTypes = {String.class, Configuration.class};
		try {
			Method m = klass.getMethod(methodName, paramTypes);
			fCommandMap.put(cmd, m);
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
			Utility.fatalError("プログラムが変です");
		} catch (SecurityException e) {
			e.printStackTrace();
			Utility.fatalError("メソッド(" + methodName +")の情報を取得できません");
		}
    }

    public String replace(String cmd, Configuration config)
		throws ReplacerNotFoundException {
		Method m = (Method )fCommandMap.get(cmd);
		if(m == null) {
			throw new ReplacerNotFoundException(cmd);
		}

		Object[] params = {cmd, config};
		try {
			return (String )m.invoke(this, params);
		} catch (Exception e) {
			e.printStackTrace();
			Utility.fatalError("プログラムに問題がありそう... (-.-;;");
		}
		return null;
    }

    public String replaceSimple(String cmd, Configuration config) {
		String value = config.get(cmd);

		if(value == null) {
			Utility.fatalError("キーワード " + cmd + "の値がありません。\n"+
							   "設定ファイルに追加してください");
		}
		return value;
    }

	public String replaceBoolean(String cmd, Configuration config) {
		String value = config.get(cmd);
		if(value == null) {
			Utility.fatalError("キーワード " + cmd + "の値がありません。\n"+
							   "設定ファイルに追加してください");
		}

		if(value.toLowerCase().equals("true")) {
			return "#t";
		} else {
			return "#f";
		}
	}
}
