package com.mogujie.ares.lib.net;

public class Test {

	public static void main(String[] args) {
		// TODO Auto-generated method stub
		String str = "2345678965435678908765467890";
		MoguHttp.uploadAudioByteFile("http://122.225.68.125:8001/", str.getBytes());
	}

}
