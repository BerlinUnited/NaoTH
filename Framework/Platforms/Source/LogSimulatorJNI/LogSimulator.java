public class LogSimulator {
   static {
	  // Load native library at runtime
      System.loadLibrary("logsimulator");
   }
 
   // 
   private native void stepForward();
   private native void stepBack();
   private native void jumpToBegin();
   private native void jumpToEnd();
   private native void jumpTo(int position);
   private native void openLogFile(String path);
   
   private native int getCurrentFrame();
   private native int getMinFrame();
   private native int getMaxFrame();
 
   // Test Driver
   public static void main(String[] args) throws Exception 
   {
		LogSimulator me = new LogSimulator();
		me.openLogFile(args[0]);
		System.out.println(args[0]);
		
		me.jumpToBegin();
		while(true)
		{
			System.in.read();
			me.stepForward();
			System.out.println("[" + me.getMinFrame() + ":" + me.getCurrentFrame() + ":" + me.getMaxFrame() + "]");
		}
   }
}