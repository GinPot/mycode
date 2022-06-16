/*****************************************************************************
*                                                                            *
*              CONFIDENTIAL VISTEON CORPORATION                              *
*                                                                            *
* This is an unpublished work of authorship, which contains trade            *
* secrets, created in 2013. Visteon Corporation owns all rights to           *
* this work and intends to maintain it in confidence to preserve             *
* its trade secret status. Visteon Corporation reserves the right,           *
* under the copyright laws of the United States or those of any              *
* other country that may have jurisdiction, to protect this work             *
* as an unpublished work, in the event of an inadvertent or                  *
* deliberate unauthorized publication. Visteon Corporation also              *
* reserves its rights under all copyright laws to protect this               *
* work as a published work, when appropriate. Those having access            *
* to this work may not copy it, use it, modify it or disclose the            *
* information contained in it without the written authorization              *
* of Visteon Corporation.                                                    *
*                                                                            *
******************************************************************************/
/*****************************************************************************

File Name        : Dbus.java
Module Name      : Media
Author           : Li, Libing
Description      : This module contains Dbus interface for java.

******************************************************************************/
package com.yfvet.javadbus;

import android.os.RemoteException;
import android.util.Log;

import java.lang.ref.WeakReference;
import java.util.HashMap;
import java.util.Map;

// Singleton class for JNI call and callback
public class Dbus {
    private static final String TAG = "JavaDbus";
    private static Dbus ourInstance; // Singleton
    private static Object mLock = null;
    private Map<String, DbusCallback> mDbusCallbacksMap;

	public static final String IpclOutToAndroid="ipcl.out.to.droid";
	private static final String IpclInFronAndroid="ipcl.in.from.droid";
	
	public static final String MN_ac_gen_rx_event ="ac_gen_rx_event";
	public static final String MN_ad_ic_rx_event ="ad_ic_rx_event";
	public static final String MN_ae_seat_rx_event ="ae_seat_rx_event";
	public static final String MN_af_ar_rx_event ="af_ar_rx_event";
	public static final String MN_ag_amblgt_rx_event ="ag_amblgt_rx_event";
	public static final String MN_ah_pedal_rx_event ="ah_pedal_rx_event";
	public static final String MN_ba_tire_rx_event ="ba_tire_rx_event";
	public static final String MN_bb_knob_rx_event ="bb_knob_rx_event";
	public static final String MN_bc_airsusp_rx_event ="bc_airsusp_rx_event";
	public static final String MN_bd_acc_rx_event ="bd_acc_rx_event";
	public static final String MN_be_hvac_rx_event ="be_hvac_rx_event";
	public static final String MN_ca_scenarios_rx_event ="bf_scenarios_rx_event";
	public static final String MN_cb_odi_rx_event ="bg_odi_rx_event";

	private static final String MN_ac_gen_tx_event ="ac_gen_tx_event";
	private static final String MN_ad_ic_tx_event ="ad_ic_tx_event";
	private static final String MN_ae_seat_tx_event ="ae_seat_tx_event";
	private static final String MN_af_ar_tx_event ="af_ar_tx_event";
	private static final String MN_ag_amblgt_tx_event ="ag_amblgt_tx_event";
	private static final String MN_ah_pedal_tx_event ="ah_pedal_tx_event";
	private static final String MN_ba_tire_tx_event ="ba_tire_tx_event";
	private static final String MN_bb_knob_tx_event ="bb_knob_tx_event";
	private static final String MN_bc_airsusp_tx_event ="bc_airsusp_tx_event";
	private static final String MN_bd_acc_tx_event ="bd_acc_tx_event";
	private static final String MN_be_hvac_tx_event ="be_hvac_tx_event";
	private static final String MN_bg_patac_tx_event ="bg_patac_tx_event";
	private static final String MN_ca_scenarios_tx_event ="ca_scenarios_tx_event";
	private static final String MN_cb_odi_tx_event ="cb_odi_tx_event";

	private static final String MN_rx_get_event ="bf_rx_get_event";
	private static final byte[] IPCL_CAN_SIGNAL_GEN_RX_EVENT = {0x40};                     
 	private static final byte[] IPCL_CAN_SIGNAL_IC_RX_EVENT = {0x41};
	private static final byte[] IPCL_CAN_SIGNAL_SEAT_RX_EVENT = {0x42};
 	private static final byte[] IPCL_CAN_SIGNAL_AR_RX_EVENT = {0x43};
 	private static final byte[] IPCL_CAN_SIGNAL_AMBLGT_RX_EVENT = {0x44};
 	private static final byte[] IPCL_CAN_SIGNAL_PEDAL_RX_EVENT = {0x45};
 	private static final byte[] IPCL_CAN_SIGNAL_TIRE_RX_EVENT = {0x46};
 	private static final byte[] IPCL_CAN_SIGNAL_KNOB_RX_EVENT = {0x47};
 	private static final byte[] IPCL_CAN_SIGNAL_AIRSUSP_RX_EVENT = {0x48};
 	private static final byte[] IPCL_CAN_SIGNAL_ACC_RX_EVENT = {0x49};
 	private static final byte[] IPCL_CAN_SIGNAL_HVAC_RX_EVENT = {0x4a};
	private static final byte[] IPCL_CAN_SIGNAL_Scenarios_RX_EVENT = {0x4b};
	private static final byte[] IPCL_CAN_SIGNAL_ODI_RX_EVENT = {0x4c};

    private native void nativeInit();

    private native void nativeSetup();

    private native boolean nativeSend(String ifName, String mbName, int len, byte[] pl);

    private native boolean nativeAddCallback(Object dbus_this, String ifName);

    private native boolean nativeDelCallback(String ifName);

    private long mNativeContext;

    // Get the Singleton
    public static synchronized Dbus getInstance() {
        if (ourInstance == null) {
            ourInstance = new Dbus();
        }
        return ourInstance;
    }

    static {  // This block will run after Constructor finish.
        System.loadLibrary("dbus_jni");
    }

    // Constructor
    private Dbus() {
        mLock = new Object();
        mDbusCallbacksMap = new HashMap<String, DbusCallback>();
        nativeInit();
        nativeSetup();
    }

    public interface DbusCallback {
        public boolean OnReceiveMessage(String ifName, String mbName, int len, byte[] pl);
    }

    public boolean Send(String ifName, String mbName, int len, byte[] pl) {
    	//Log.d(TAG,"Send--ifName = " + ifName + " mbName = " + mbName + "len = " + len);
    	/*for(int i = 0 ; i < len ;i++){
			Log.d(TAG,"i = " + i + " value = " + pl[i]);
		}*/
        synchronized (mLock) {
			//Log.d(TAG,"nativeSend--ifName = " + ifName + " mbName = " + mbName + "len = " + len);
            return nativeSend(ifName, mbName, len, pl);
        }
    }

	// send dbus msg
	public boolean SendGen(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_ac_gen_tx_event,len,pl);
	}
	
	public boolean SendIC(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_ad_ic_tx_event,len,pl);
	}
	
	public boolean SendSeat(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_ae_seat_tx_event,len,pl);
	}
	
	public boolean SendAR(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_af_ar_tx_event,len,pl);
	}
	
	public boolean SendAmb(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_ag_amblgt_tx_event,len,pl);
	}

	public boolean SendPedal(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_ah_pedal_tx_event,len,pl);
	}

	public boolean SendTire(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_ba_tire_tx_event,len,pl);
	}

	public boolean SendKnob(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_bb_knob_tx_event,len,pl);
	}

	public boolean SendAirSuspend(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_bc_airsusp_tx_event,len,pl);
	}

	public boolean SendAcc(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_bd_acc_tx_event,len,pl);
	}
	
	public boolean SendHvac(int len, byte[] pl) {
	        return Send(IpclInFronAndroid,MN_be_hvac_tx_event,len,pl);
	}

	public boolean SendPatac(int len, byte[] pl) {
		return Send(IpclInFronAndroid,MN_bg_patac_tx_event,len,pl);
	}

	public boolean SendScenarios(int len, byte[] pl) {
		return Send(IpclInFronAndroid,MN_ca_scenarios_tx_event,len,pl);
	}

	public boolean SendODI(int len, byte[] pl) {
		return Send(IpclInFronAndroid,MN_cb_odi_tx_event,len,pl);
	}

	// get dbus msg , return in DbusCallback
    public boolean GetGen() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_GEN_RX_EVENT);
	}
	
	public boolean GetIC() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_IC_RX_EVENT);
	}
	
	public boolean GetSeat() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_SEAT_RX_EVENT);
	}
	
	public boolean GetAR() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_AR_RX_EVENT);
	}
	
	public boolean GetAmb() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_AMBLGT_RX_EVENT);
	}

	public boolean GetPedal() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_PEDAL_RX_EVENT);
	}

	public boolean GetTire() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_TIRE_RX_EVENT);
	}

	public boolean GetKnob() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_KNOB_RX_EVENT);
	}

	public boolean GetAirSuspend() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_AIRSUSP_RX_EVENT);
	}

	public boolean GetAcc() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_ACC_RX_EVENT);
	}
	
	public boolean GetHvac() {
	        return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_HVAC_RX_EVENT);
	}

	public boolean GetScenarios() {
		return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_Scenarios_RX_EVENT);
	}

	public boolean GetOdi() {
		return Send(IpclInFronAndroid,MN_rx_get_event,1,IPCL_CAN_SIGNAL_ODI_RX_EVENT);
	}

	//Add DbusCallback 
	/*public boolean AddGenReceiveCallback(String pkName,DbusCallback cb) {
	        return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ac_gen_rx_event,cb);
	}
	
	public boolean AddICReceiveCallback(String pkName,DbusCallback cb) {
	        return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ad_ic_rx_event,cb);
	}	

	public boolean AddSeatReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ae_seat_rx_event,cb);
	}
	public boolean AddARReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_af_ar_rx_event,cb);
	}
	public boolean AddAmbReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ag_amblgt_rx_event,cb);
	}
	public boolean AddPedalReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ah_pedal_rx_event,cb);
	}
	public boolean AddTireReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ba_tire_rx_event,cb);
	}
	public boolean AddKnobReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_bb_knob_rx_event,cb);
	}
	public boolean AddAirSuspendReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_bc_airsusp_rx_event,cb);
	}
	public boolean AddAccReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_bd_acc_rx_event,cb);
	}
	public boolean AddHvacReceiveCallback(String pkName,DbusCallback cb) {
			return AddReceiveCallback(pkName,IpclOutToAndroid,MN_be_hvac_rx_event,cb);
	}
	public boolean AddScenariosReceiveCallback(String pkName, com.yfvet.javadbus.Dbus.DbusCallback cb) {
		return AddReceiveCallback(pkName,IpclOutToAndroid,MN_ca_scenarios_rx_event,cb);
	}
	public boolean AddOdiReceiveCallback(String pkName, com.yfvet.javadbus.Dbus.DbusCallback cb) {
		return AddReceiveCallback(pkName,IpclOutToAndroid,MN_cb_odi_rx_event,cb);
	}*/



	public boolean AddReceiveCallback(String ifName, DbusCallback cb) {
        Log.d(TAG, "add DbusCallback->" + cb + ", ifName:" + ifName);
        // do Native AddReceiveCallback
        synchronized (mDbusCallbacksMap) {
            mDbusCallbacksMap.put(ifName, cb);
        }
        if (nativeAddCallback(new WeakReference<Dbus>(this), ifName)) {
            return true;
        } else {
            DelReceiveCallback(ifName);
            return false;
        }


    }

    public boolean DelReceiveCallback(String ifName) {
        Log.d(TAG, "DelCallback->" + ifName);
        synchronized (mDbusCallbacksMap) {
            if (mDbusCallbacksMap.containsKey(ifName)) {
                mDbusCallbacksMap.remove(ifName);
            }
        }

        // do Native DelReceiveCallback
        return nativeDelCallback(ifName);
    }

    // This function will be called by C++ when received a new message
    private boolean OnReceive(Object dbus_ref, String ifName, String mbName, int len, byte[] pl) {
        Log.d(TAG, "OnReceive " + ifName + ", " + mbName);
        synchronized (mLock) {
			if(mbName!=null&&mbName.equals("abc")) return true;
			// Call the Callback to handle this message
            Dbus mDbus = (Dbus) ((WeakReference) dbus_ref).get();
            if (mDbus == null) {
                Log.e(TAG, "mDbus is null");
                return false;
            }
            Map<String, DbusCallback> mCallbacksMap = mDbus.mDbusCallbacksMap;
            if (mCallbacksMap == null) {
                Log.e(TAG, "mCallBacksMap is Null");
                return false;
            }

            DbusCallback mCallback = mCallbacksMap.get(ifName);
            if (mCallback == null) {
                Log.e(TAG, "mCallback is null for ifname : " + ifName);
                return false;
            }
            return mCallback.OnReceiveMessage(ifName, mbName, len, pl);

        }
    }
}
