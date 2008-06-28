import java.lang.*;

public class NetworkCommand extends TemplateCommand {
	public final static String NETMASK = "NetMask";
	public final static String NETWORK = "Network";
	public final static String PORT_OFFSET = "PortOffset";

	public NetworkCommand() {
		super();

		// ここでは replace の登録を行わない
		// 必要ならサブクラスで行うこと
	}

	public String getIPAddress(Configuration config, String role) {
		String network = config.get(NETWORK);

		// 本当は、netmask に従って IP アドレスの文字列を作る
		// 必要があるが、ここでは手抜きしてクラスCと仮定

		int index = network.lastIndexOf('.');
		return network.substring(0, index + 1) + config.get(role);
	}
	
	public int getPortOffset(Configuration config, String role) {
		String portOffset = config.get(PORT_OFFSET);
		return Integer.parseInt(portOffset);
	}

}
