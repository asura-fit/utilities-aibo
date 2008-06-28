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
			Utility.fatalError("�ץ���ब�ѤǤ�");
		} catch (SecurityException e) {
			e.printStackTrace();
			Utility.fatalError("�᥽�å�(" + methodName +")�ξ��������Ǥ��ޤ���");
		}
    }

    protected void registerCommand(Class klass, String cmd, String methodName){
		Class[] paramTypes = {String.class, Configuration.class};
		try {
			Method m = klass.getMethod(methodName, paramTypes);
			fCommandMap.put(cmd, m);
		} catch (NoSuchMethodException e) {
			e.printStackTrace();
			Utility.fatalError("�ץ���ब�ѤǤ�");
		} catch (SecurityException e) {
			e.printStackTrace();
			Utility.fatalError("�᥽�å�(" + methodName +")�ξ��������Ǥ��ޤ���");
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
			Utility.fatalError("�ץ��������꤬���ꤽ��... (-.-;;");
		}
		return null;
    }

    public String replaceSimple(String cmd, Configuration config) {
		String value = config.get(cmd);

		if(value == null) {
			Utility.fatalError("������� " + cmd + "���ͤ�����ޤ���\n"+
							   "����ե�������ɲä��Ƥ�������");
		}
		return value;
    }

	public String replaceBoolean(String cmd, Configuration config) {
		String value = config.get(cmd);
		if(value == null) {
			Utility.fatalError("������� " + cmd + "���ͤ�����ޤ���\n"+
							   "����ե�������ɲä��Ƥ�������");
		}

		if(value.toLowerCase().equals("true")) {
			return "#t";
		} else {
			return "#f";
		}
	}
}
