package lib.zxtcode.com.mylib;

/**
 * Created by zxt lan4627@Gmail.com on 2018/1/24.
 */

public class AudioPartInfo {

    private String filePath;
    private long audioTime;

    public AudioPartInfo(String filePath, long audioTime) {
        this.filePath = filePath;
        this.audioTime = audioTime;
    }

    public String getFilePath() {
        return filePath;
    }

    public void setFilePath(String filePath) {
        this.filePath = filePath;
    }

    public long getAudioTime() {
        return audioTime;
    }

    public void setAudioTime(long audioTime) {
        this.audioTime = audioTime;
    }
}
