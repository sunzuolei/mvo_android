package com.example.visodo;

import android.app.Activity;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BitmapFactory;
import android.os.Bundle;
import android.util.Log;

import android.view.SurfaceView;
import android.widget.Button;
import android.widget.ImageView;
import android.widget.TextView;

import org.opencv.android.BaseLoaderCallback;
import org.opencv.android.CameraBridgeViewBase;
import org.opencv.android.JavaCameraView;
import org.opencv.android.LoaderCallbackInterface;
import org.opencv.android.OpenCVLoader;
import org.opencv.android.Utils;
import org.opencv.core.CvType;
import org.opencv.core.Mat;
import org.opencv.core.Scalar;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;

public class MainActivity extends Activity implements CameraBridgeViewBase.CvCameraViewListener2 {

	double xx = 0d;
	double yy = 0d;
	double zz = 0d;
	private ImageView show;
	private String TAG = MainActivity.class.getSimpleName();
	private CameraBridgeViewBase mOpenCvCameraView;
	private Mat mRgba;
	private Mat firstPic;
	private Mat secondPic;
	private Mat afterPic;
	private Bitmap bitmap;
	private TextView result;
	private Button btnChange;
	private boolean first = true;
	private boolean second = false;
	private boolean third = false;
	private int i = 2;
	private boolean isFromCamera = true;
	private int width;
	private int height;

	private BaseLoaderCallback mLoaderCallback = new BaseLoaderCallback(this) {
		@Override
		public void onManagerConnected(int status) {
			switch (status) {
				case LoaderCallbackInterface.SUCCESS: {
					Log.i(TAG, "OpenCV loaded successfully");
					mOpenCvCameraView.enableView();
				}
				break;
				default: {
					super.onManagerConnected(status);
				}
				break;
			}
		}
	};

	@Override
	protected void onCreate(Bundle savedInstanceState) {
		super.onCreate(savedInstanceState);
		setContentView(R.layout.activity_main);
		show = (ImageView) findViewById(R.id.tv_show);
		result = (TextView) findViewById(R.id.tv_result);
		btnChange = (Button) findViewById(R.id.btn_change);
		btnChange.setOnClickListener(v -> {
			mRgba = new Mat(height, width, CvType.CV_8UC4, Scalar.all(255));
			first = true;
			second = false;
			third = false;
			i = 2;
			if (isFromCamera) {
				isFromCamera = false;
				btnChange.setText("Local");
			} else {
				isFromCamera = true;
				btnChange.setText("Camera");
			}
		});
		mOpenCvCameraView = /*(CameraBridgeViewBase)*/(JavaCameraView) findViewById(R.id.color_blob_detection_activity_surface_view);
		mOpenCvCameraView.setVisibility(SurfaceView.VISIBLE);
		mOpenCvCameraView.setCvCameraViewListener(this);
		IntentFilter filter = new IntentFilter();
		filter.addAction("bitmap");
//		show.setVisibility(View.GONE);
	}

	private void showPic() {
		show.setImageBitmap(bitmap);
	}

	@Override
	public void onCameraViewStarted(int width, int height) {
		this.width = width;
		this.height = height;
		mRgba = new Mat(height, width, CvType.CV_8UC4, Scalar.all(255));
	}

	@Override
	public void onCameraViewStopped() {
		mRgba.release();
	}

	@Override
	public void onPause() {
		super.onPause();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	@Override
	public void onResume() {
		super.onResume();
		if (!OpenCVLoader.initDebug()) {
			Log.i(TAG, "Internal OpenCV library not found. Using OpenCV Manager for initialization");
			OpenCVLoader.initAsync(OpenCVLoader.OPENCV_VERSION, this, mLoaderCallback);
		} else {
			Log.i(TAG, "OpenCV library found inside package. Using it!");
			mLoaderCallback.onManagerConnected(LoaderCallbackInterface.SUCCESS);
		}
	}

	@Override
	public void onDestroy() {
		super.onDestroy();
		if (mOpenCvCameraView != null)
			mOpenCvCameraView.disableView();
	}

	private Bitmap getDiskBitmap(String pathString) {
		Bitmap bitmap = null;
		try {
			File file = new File(pathString);
			if (file.exists()) {
				bitmap = BitmapFactory.decodeFile(pathString);
			}
		} catch (Exception ignored) {
		}
		return bitmap;
	}

	@Override
	public Mat onCameraFrame(CameraBridgeViewBase.CvCameraViewFrame/*Mat*/ inputFrame) {
		if (first) {
			firstPic = inputFrame.rgba();
			first = false;
			second = true;
			Log.i(TAG, "first success");
		}
		if (second) {
			secondPic = inputFrame.rgba();
			Log.i(TAG, secondPic.type() + "" + secondPic.channels() + secondPic);
			second = false;
			third = true;
			LibVisodo.init(firstPic.getNativeObjAddr(),
					secondPic.getNativeObjAddr(), isFromCamera);
			Log.i(TAG, "second success");
		}
		if (third) {
			String pathString = String.format(getResources().getString(R.string.path), i);
			final Bitmap firstBitmap = getDiskBitmap(pathString);
			afterPic = inputFrame.rgba();
			if (isFromCamera) {
				LibVisodo.FindFeatures(afterPic.getNativeObjAddr());
			} else {
				Utils.bitmapToMat(firstBitmap, afterPic);
				LibVisodo.FindFeatures(afterPic.getNativeObjAddr());
			}
			bitmap = Bitmap.createBitmap(inputFrame.rgba().width(), inputFrame.rgba().height(), Bitmap.Config.ARGB_8888);
			new Thread(() -> {
				Utils.matToBitmap(afterPic, bitmap);
				if (bitmap != null) {
					runOnUiThread(() -> {
//								if(!isFromCamera){
//								bitmap = firstBitmap;
//								}
						showPic();
						Log.i(TAG, "show pic");
					});
				}
			}).start();

			try {
				if (!isFromCamera) {
					if (firstBitmap != null) {
						double[] result = LibVisodo.start(mRgba.getNativeObjAddr(),
								inputFrame.rgba().getNativeObjAddr(), i, xx, yy, zz, isFromCamera);
						Log.i(TAG, result[0] + "");
						invalidResult(result);
					} else {

					}
				} else {
					double[] result = LibVisodo.start(mRgba.getNativeObjAddr(),
							afterPic.getNativeObjAddr(), i, xx, yy, zz, isFromCamera);
					Log.i(TAG, result[0] + "");
					invalidResult(result);
				}
			} catch (Exception e) {
				Log.e(TAG, "FAIL" + e);
			}
			i++;
			Log.i(TAG, "third success");
		}
		return mRgba;
	}

	private void invalidResult(double[] result2) {
		result.setText("Coordinates:x=" + CutUtil.Cut(result2[0] + "") + "m  y=" + CutUtil.Cut(result2[1] + "")
				+ "m  z=" + CutUtil.Cut(result2[2] + "") + "m");
	}

	public void saveBitmap(Bitmap bm) {
		Log.i(TAG, "save Picture");
		File f = new File("/storage/emulated/0/image/", "000002.png");
		if (f.exists()) {
			f.delete();
		}
		try {
			FileOutputStream out = new FileOutputStream(f);
			bm.compress(Bitmap.CompressFormat.PNG, 90, out);
			out.flush();
			out.close();
			Log.w(TAG, "Already saved");
		} catch (IOException e) {
			e.printStackTrace();
		}
	}
}
