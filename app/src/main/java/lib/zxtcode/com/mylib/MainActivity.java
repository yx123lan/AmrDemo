package lib.zxtcode.com.mylib;

import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.View;
import android.widget.Button;
import android.widget.TextView;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    private File mAmrFile;
    private Button mBtn;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mBtn = findViewById(R.id.btn);
        mAmrFile = new File(getExternalCacheDir().getPath(), "Kanon.amr");
        copyAmrFileToCacheDir(mAmrFile.getAbsolutePath());
        mBtn.setOnClickListener(new OnClickSplitListener());
    }

    private void copyAmrFileToCacheDir(String amrFilePath) {
        try {
            InputStream is = getAssets().open("Kanon.amr");
            FileOutputStream fos = new FileOutputStream(amrFilePath);
            int length = 2048;
            byte[] buffer = new byte[length];
            int count = 0;
            while ((count = is.read(buffer, 0, length)) != -1) {
                fos.write(buffer, 0, count);
            }
            is.close();
            fos.close();
        } catch (IOException e) {
            e.printStackTrace();
        }
    }

    class OnClickSplitListener implements View.OnClickListener {

        @Override
        public void onClick(View view) {
            File part1 = new File(getExternalCacheDir().getPath(), "kanon_p1.amr");
            File part2 = new File(getExternalCacheDir().getPath(), "kanon_p2.amr");
            AudioPartInfo[] infoArray = new AudioPartInfo[2];
            infoArray[0] = new AudioPartInfo(part1.getPath(), 60 * 1000);
            infoArray[1] = new AudioPartInfo(part2.getPath(), 120 * 1000);
            try {
                AudioTool.splitAMRFiles(infoArray, mAmrFile.getAbsolutePath());
                mAmrFile.delete();

                // 设置为点击合并文件
                String[] inputFileArray = new String[2];
                inputFileArray[0] = part1.getPath();
                inputFileArray[1] = part2.getPath();
                mBtn.setText(R.string.join);
                mBtn.setOnClickListener(new OnClickJoinListener(inputFileArray));
            } catch (IOException e) {
                e.printStackTrace();
            }
        }
    }

    class OnClickJoinListener implements View.OnClickListener {

        private String[] mInputFileArray;

        OnClickJoinListener(String[] inputFileArray) {
            mInputFileArray = inputFileArray;
        }

        @Override
        public void onClick(View view) {
            // 重新合并在一起
            try {
                AudioTool.joinAMRFiles(mInputFileArray, mAmrFile.getAbsolutePath());
                mBtn.setText(R.string.split);
                mBtn.setOnClickListener(new OnClickSplitListener());
            } catch (IOException e) {
                e.printStackTrace();
            } catch (JoinFileFailureException e) {
                e.printStackTrace();
            }
        }
    }
}
