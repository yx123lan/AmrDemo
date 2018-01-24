package lib.zxtcode.com.mylib;

import java.io.IOException;

/**
 * Created by zxt on 2018/1/24.
 */

public class AudioTool {

    static {
        System.loadLibrary("native-lib");
    }

    public static native void joinAMRFiles(String[] inputFileArray, String outputFile) throws IOException, JoinFileFailureException;
    public static native void splitAMRFiles(AudioPartInfo[] outputInfoArray, String inputFile) throws IOException;
    public static native int isAMRFile(String inputFile);

}
