package com.example.visodo;

public class LibVisodo {
	static {
		System.loadLibrary("opencv_java4");
		System.loadLibrary("visodo");
	}

	public static native String init(long firstPic, long secondPic, boolean isFromCamera);

	public static native double[] start(long matAddrRgba, long afterPic, int i, double xx, double yy, double zz, boolean isFromCamera);

	public static native void FindFeatures(long matAddrRgba);
}
