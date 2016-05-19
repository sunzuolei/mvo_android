# mvo_android
The monocular visual odometry implemented on an Android phone.
### 1.Dependency
* Eclipse with ADT and android-ndk-r10e;
* OpenCV4Android 3.0.0 or 3.1.0;

  [Here to download OpenCV4Android](http://opencv.org/downloads.html)
* OpenCV Manger

### 2.BuildProject
To build mvo_android, you can simply import cloned project into eclipse.I've already imported and set opencv3.1.0 as library,so you can jump this step.After that,you need to change the path to your OpenCV.mk in Android.mk. Config and run ndk-build . 

### 3.Function
Through this application , you can calcuate the position and attitude of camera.Moreover,it provides feature detection constantly.

* Camera Mode

  Using mobile camera to get image inputFrame.
  ##### camera mode runtime screenshot
  ![Camera Mode Runtime Screenshot](http://7xqxgd.com1.z0.glb.clouddn.com/camrea_%E5%89%AF%E6%9C%AC.png)
* Local Mode

  Get inputFrame through local storage directory.
  ##### local mode runtime screenshot
  ![Local Mode Runtime Screenshot](http://7xqxgd.com1.z0.glb.clouddn.com/local_%E5%89%AF%E6%9C%AC(1).png)

