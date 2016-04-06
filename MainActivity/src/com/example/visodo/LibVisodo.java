package com.example.visodo;

public class LibVisodo {
	static {  
		System.loadLibrary("opencv_java3");
        System. loadLibrary( "visodo");  
       }  
         public static native String start(long matAddrRgba,long afterPic,int i); 
         
         public static native String init(long firstPic,long secondPic);
}
