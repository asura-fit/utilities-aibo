import java.lang.*;

public class NetworkCommand extends TemplateCommand {
	public final static String NETMASK = "NetMask";
	public final static String NETWORK = "Network";
	public final static String PORT_OFFSET = "PortOffset";

	public NetworkCommand() {
		super();

		// �����ł� replace �̓o�^���s��Ȃ�
		// �K�v�Ȃ�T�u�N���X�ōs������
	}

	public String getIPAddress(Configuration config, String role) {
		String network = config.get(NETWORK);

		// �{���́Anetmask �ɏ]���� IP �A�h���X�̕���������
		// �K�v�����邪�A�����ł͎蔲�����ăN���XC�Ɖ���

		int index = network.lastIndexOf('.');
		return network.substring(0, index + 1) + config.get(role);
	}
	
	public int getPortOffset(Configuration config, String role) {
		String portOffset = config.get(PORT_OFFSET);
		return Integer.parseInt(portOffset);
	}

}
