# mvo_android
The monocular visual odometry implemented on an Android phone.
### 1.Dependency
* Android Studio 3.5
* OpenCV4Android sdk 3.4.0 or 4.1.0;

### 2.BuildProject
To build mvo_android, you can simply import cloned project into studio.
You need to change the path to your OpenCV_sdk in Android.mk & build.gradle. 
Config and run ndk-build . 

### 3.Function
Through this application , you can calculate the position and attitude of camera.Moreover,it provides feature detection constantly.

* Camera Mode

  Using mobile camera to get image inputFrame.
  ##### camera mode runtime screenshot
  ![Camera Mode Runtime Screenshot](http://7xqxgd.com1.z0.glb.clouddn.com/camrea_%E5%89%AF%E6%9C%AC.png)
* Local Mode

  Get inputFrame through local storage directory.
  ##### local mode runtime screenshot
  ![Local Mode Runtime Screenshot](http://7xqxgd.com1.z0.glb.clouddn.com/Screenshot_2016-06-06-18-17-52.png?imageView/2/w/619/q/90)
  
  The KITTI benchmark data* is used for MVO test.    
  *KITTI data is well known as benchmark data for computer vision algorithm evaluation,it
  is available at http://www.cvlibs.net/datasets/kitti/ 

### 4.Demo
  [Here is the video demo for this application](http://v.youku.com/v_show/id_XMTU5OTQ5Njk0NA==.html?x&sharefrom=android)
  
### 5.APK(arm64-v8a)
  If you want to try Visdo simply,you can download and install Visodo-release.apk on your android phone 

