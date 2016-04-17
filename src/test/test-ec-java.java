import com.enjoyars.*;

public class rf21x_test {
	static {
		try {
			System.loadLibrary("rf21x-java");
		} catch (UnsatisfiedLinkError e) {
			System.err.println("Native code library failed to load rf21-java.\n"
					+ e);
			System.exit(1);
		}
	}

	public static void main(String argv[]) {
		System.out.println("Begin");
		RF21xDevice rf = new RF21xDevice();
		if (rf.open(rf21xConstants.RF21X_DT_RF218, "hid://", 1, 40) == false) {
			System.out.println("Fail to open device.");
			return;
		}
		try {
			Thread.sleep(2000);
		} catch (InterruptedException e) {
			// TODO Auto-generated catch block
			e.printStackTrace();
		}
		if (rf.startQuiz(rf21xConstants.RF21X_QT_Single, 2, 8) == false) {
			System.out.println("Fail to start quiz.");
			return;
		}
		RF21xMessage message = new RF21xMessage();
		while (true) {
			if (rf.getMessage(message)) {
				if (message.getMessageType() == rf21xConstants.RF21X_MT_Teacher) {
					System.out.printf("Teacher: %s\n", message.getData());
				} else if (message.getMessageType() == rf21xConstants.RF21X_MT_Student) {
					System.out
							.printf("Student %d for current question: %s\n",
									message.getKeypadId(), message.getData());
				} else if (message.getMessageType() == rf21xConstants.RF21X_MT_SetId) {
					System.out.printf("Set ID %d\n", message.getKeypadId());
				}

				RF21xMessageRawData rawData = message.getRawData();
				System.out.printf("Raw data: ");
				for (int i = 0; i < rawData.size(); ++i) {
					System.out.printf("%02X ", rawData.get(i));
				}
				System.out.printf("\n");
			}
			else {
				try {
					Thread.sleep(100);
				} catch (InterruptedException e) {
					// TODO Auto-generated catch block
					e.printStackTrace();
				}
			}
		}
	}
}
