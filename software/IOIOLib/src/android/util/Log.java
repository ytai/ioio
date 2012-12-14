package android.util;

public class Log {
  public static int v(String tag, String msg) {
    System.out.println("VERB - " + tag + ": " + msg);
    return 0;
  }
  public static int v(String tag, String msg, Throwable tr) {
    System.out.println("VERB - " + tag + ": " + msg);
    tr.printStackTrace(System.out);
   return 0;
  }
  
  
  public static int d(String tag, String msg) {
    System.out.println("DEBUG - " + tag + ": " + msg);
    return 0;
  }
  public static int d(String tag, String msg, Throwable tr) {
    System.out.println("DEBUG - " + tag + ": " + msg);
    tr.printStackTrace(System.out);
   return 0;
  }
  
  
  public static int i(String tag, String msg) {
    System.out.println("INFO - " + tag + ": " + msg);
    return 0;
  }
  public static int i(String tag, String msg, Throwable tr) {
    System.out.println("INFO - " + tag + ": " + msg);
    tr.printStackTrace(System.out);
   return 0;
  }
  
  
  public static int w(String tag, String msg) {
    System.err.println("WARN - " + tag + ": " + msg);
    return 0;
  }
  public static int w(String tag, String msg, Throwable tr) {
    System.err.println("WARN - " + tag + ": " + msg);
    tr.printStackTrace(System.err);
   return 0;
  }
  
  
  public static int e(String tag, String msg) {
    System.err.println("ERR - " + tag + ": " + msg);
    return 0;
  }
  public static int e(String tag, String msg, Throwable tr) {
    System.err.println("ERR - " + tag + ": " + msg);
    tr.printStackTrace(System.err);
   return 0;
  }
}
