import java.lang.*;

public class Utility {
	static void fatalError(String msg) {
		System.err.println(msg);
		System.exit(-1);
	}

	static void error(String msg) {
		System.err.println(msg);
	}

	static void usagePrinter(String[] msgs) {
		for(int i = 0; i < msgs.length; i++) {
			System.out.println(msgs[i]);
		}
	}
}

