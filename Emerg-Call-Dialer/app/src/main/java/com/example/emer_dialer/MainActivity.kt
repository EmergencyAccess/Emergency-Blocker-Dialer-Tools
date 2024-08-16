package com.example.emer_dialer

import android.Manifest
import android.content.pm.PackageManager
import android.location.Address
import android.location.Geocoder
import android.location.Location
import androidx.core.app.ActivityCompat
import androidx.core.content.ContextCompat
import com.google.android.gms.location.FusedLocationProviderClient
import com.google.android.gms.location.LocationServices
import java.io.IOException
import java.util.Locale
import java.io.InputStreamReader
import com.google.gson.Gson
import com.google.gson.reflect.TypeToken


import android.content.Context
import android.os.Build
import android.os.Bundle
import android.view.View
import androidx.annotation.RequiresApi
import androidx.appcompat.app.AppCompatActivity

import android.content.Intent
import android.net.Uri
import android.util.Log

import android.net.wifi.WifiInfo
import android.net.wifi.WifiManager
import android.telephony.*

import android.content.BroadcastReceiver
import android.content.IntentFilter

import android.os.Handler
import android.os.Looper
import android.widget.*

class MainActivity : AppCompatActivity() {

    private lateinit var enteredNumberTextView: TextView
    private lateinit var signalStrengthTextView: TextView
    private lateinit var telephonyManager: TelephonyManager
    private lateinit var phoneStateListener: PhoneStateListener
    private lateinit var wifiManager: WifiManager
    private lateinit var wifisignalStrengthTextView: TextView
    private lateinit var cityNameTextView: TextView
    private lateinit var switchCellTextView: TextView
    private lateinit var switchWifiTextView: TextView
    private lateinit var fusedLocationClient: FusedLocationProviderClient
    private lateinit var countyPSAPMap: Map<String, String>
    private lateinit var callButton: ImageButton

    var best_pci: Int? = 0
    var best_earfcn: Int? = 0
    var best_signal = -999
    var is_switch = false
    var current_strength = 0;
    private val PREFS_NAME = "prefs"
    private val THRESHOLD_KEY = "threshold"
    private var threshold = 100  // Default value

    private var countyName: String = "0"
    private var PSAPNumber: String = "0"

    // Update WiFi signal strength in real-time
    private val wifiReceiver = object : BroadcastReceiver() {
        override fun onReceive(context: Context?, intent: Intent?) {

            checkAndDisplayWifiSignalStrength()
        }
    }

    override fun onCreate(savedInstanceState: Bundle?) {
        super.onCreate(savedInstanceState)

        // Load city-number pairs from the configuration file
        countyPSAPMap = loadCountyPSAPMap()

        setContentView(R.layout.activity_main)
        wifiManager = applicationContext.getSystemService(Context.WIFI_SERVICE) as WifiManager
        wifisignalStrengthTextView = findViewById(R.id.wifisignalStrengthTextView)
        cityNameTextView = findViewById(R.id.cityNameTextView)
        registerReceiver(wifiReceiver, IntentFilter(WifiManager.RSSI_CHANGED_ACTION))
        fusedLocationClient = LocationServices.getFusedLocationProviderClient(this)
        val thresholdInput = findViewById<EditText>(R.id.threshold_input)
        val saveButton = findViewById<Button>(R.id.save_button)

        // Retrieve the stored threshold value
        val sharedPreferences = getSharedPreferences(PREFS_NAME, Context.MODE_PRIVATE)
        threshold = sharedPreferences.getInt(THRESHOLD_KEY, threshold)
        // Display the current threshold value as hint
        thresholdInput.hint = "Current threshold: $threshold"
        saveButton.setOnClickListener {
            val newThreshold = thresholdInput.text.toString().toIntOrNull()
            if (newThreshold != null) {
                threshold = newThreshold
                with(sharedPreferences.edit()) {
                    putInt(THRESHOLD_KEY, threshold)
                    apply()
                }
                thresholdInput.hint = "Current threshold: $threshold"
            }
        }

        // Check permission before initial location inquiry
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_FINE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.ACCESS_FINE_LOCATION), LOCATION_PERMISSION_REQUEST_CODE)
        }
        if(ContextCompat.checkSelfPermission(this, Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
            ActivityCompat.requestPermissions(this, arrayOf(Manifest.permission.ACCESS_COARSE_LOCATION), LOCATION_PERMISSION_REQUEST_CODE)
        }
        getLastLocation()


        val cityNamehandler = Handler(Looper.getMainLooper())
        val cityNamerunnable = object : Runnable {
            override fun run() {
                getLastLocation()
                cityNamehandler.postDelayed(this, 1000)
            }
        }
        cityNamehandler.postDelayed(cityNamerunnable, 1000)


        checkAndDisplayWifiSignalStrength()
        val handler = Handler(Looper.getMainLooper())
        val runnable = object : Runnable {
            override fun run() {
                checkAndDisplayWifiSignalStrength()
                // Your function to run every second
                println("Function executed at ${System.currentTimeMillis()}")
                handler.postDelayed(this, 1000)
            }
        }
        handler.postDelayed(runnable, 1000)



        signalStrengthTextView = findViewById(R.id.signalStrengthTextView)
        telephonyManager = getSystemService(Context.TELEPHONY_SERVICE) as TelephonyManager

        phoneStateListener = object : PhoneStateListener() {
            @RequiresApi(Build.VERSION_CODES.Q)
            override fun onSignalStrengthsChanged(signalStrength: SignalStrength) {
                super.onSignalStrengthsChanged(signalStrength)
                updateSignalStrengthText(signalStrength, threshold)

            }
        }
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS)
        enteredNumberTextView = findViewById(R.id.enteredNumberTextView)
        callButton = findViewById(R.id.callButton)
        callButton.setOnClickListener { view ->
//            val enteredPhoneNumber = enteredNumberTextView.text.toString()
            onCallButtonClick(view, PSAPNumber)
        }


    }
    data class CellIdentityLteInfo(
        val mCi: Int?,
        val mPci: Int?,
        val mTac: Int?,
        val mEarfcn: Int?,
        val mBands: List<String>?,
        val mBandwidth: Int?,
        val mMcc: String?,
        val mMnc: String?,
        val mAlphaLong: String?,
        val mAlphaShort: String?,
        val mAdditionalPlmns: Map<String, String>?,
        val mCsgInfo: String?
    )
    fun parseCellIdentityLte(input: String): CellIdentityLteInfo {
        val keyValuePairs = input
            .replace("CellIdentityLte:{", "") // Remove prefix
            .replace("}", "") // Remove suffix
            .split(" ")
            .filter { it.isNotEmpty() } // Remove empty elements

        val cellIdentityMap = mutableMapOf<String, Any?>()

        for (pair in keyValuePairs) {
            val arrayList = pair.split("=")
            var separated = arrayList.toTypedArray()
            if(separated.size == 2){
                if(separated[1] == "null"){
                    cellIdentityMap[separated[0]] = null
                }
                if(separated[0] == "mBands" || separated[0] == "mAdditionalPlmns"){
                    cellIdentityMap[separated[0]] = 0
                }
                else{
                    cellIdentityMap[separated[0]] = separated[1].toIntOrNull() ?: separated[1]
                }
            }
//

        }

        return CellIdentityLteInfo(
            mCi = cellIdentityMap["mCi"] as? Int,
            mPci = cellIdentityMap["mPci"] as? Int,
            mTac = cellIdentityMap["mTac"] as? Int,
            mEarfcn = cellIdentityMap["mEarfcn"] as? Int,
            mBands = cellIdentityMap["mBands"] as? List<String>,
            mBandwidth = cellIdentityMap["mBandwidth"] as? Int,
            mMcc = cellIdentityMap["mMcc"] as? String,
            mMnc = cellIdentityMap["mMnc"] as? String,
            mAlphaLong = cellIdentityMap["mAlphaLong"] as? String,
            mAlphaShort = cellIdentityMap["mAlphaShort"] as? String,
            mAdditionalPlmns = cellIdentityMap["mAdditionalPlmns"] as? Map<String, String>,
            mCsgInfo = cellIdentityMap["mCsgInfo"] as? String
        )
    }

    data class bestInfo(
        val signal: Int,
        val Earfcn: Int?,
        val Pci: Int?,
        val is_switch: Int
    )
    data class mywifiInfo(
        val strength: Int,
        val level: Int
    )
    private fun measureNeighboringSignalStrength(): bestInfo {
        if (Build.VERSION.SDK_INT < Build.VERSION_CODES.Q) {

        } else {
            val neighboringCellInfoList = telephonyManager.getAllCellInfo()

            for (neighboringCellInfo in neighboringCellInfoList) {
                // Get signal strength in dBm
                val signalStrengthValue = neighboringCellInfo.cellSignalStrength.dbm
                val cellIdentityInfo = parseCellIdentityLte(neighboringCellInfo.cellIdentity.toString())
                if(signalStrengthValue > best_signal){
                    best_signal = signalStrengthValue
                    best_earfcn = cellIdentityInfo.mEarfcn
                    best_pci = cellIdentityInfo.mPci

                }
                Log.d(
                    "SignalStrengthActivity",
                    "Cell ID: ${neighboringCellInfo.cellIdentity}, Signal Strength: $signalStrengthValue dBm\n  TEST:best pci: ${best_pci}, best signal: ${best_signal}"
                )
            }
        }
        return bestInfo(best_signal, best_earfcn, best_pci, 0)
    }

    private fun checkAndDisplayWifiSignalStrength(): mywifiInfo? {
        val wifiInfo: WifiInfo? = wifiManager.connectionInfo
        if (wifiInfo != null) {
            val signalStrength = wifiInfo.rssi // Signal strength in dBm
            val signalLevel = WifiManager.calculateSignalLevel(signalStrength, 5) // Signal level (0-4)
            wifisignalStrengthTextView.text = "WiFi: $signalStrength dBm\nSignal Level: $signalLevel/4"
            return mywifiInfo(signalStrength, signalLevel)
        } else {
            wifisignalStrengthTextView.text = "Unable to retrieve Wi-Fi information"
            return mywifiInfo(0, 0)
        }
    }

//su shell -c 'service call phone 193 i32 1 i32 1'
    private fun adbDisableSIM() {
        try {
            val process = Runtime.getRuntime().exec(arrayOf("su", "shell", "-c", "service call phone 193 i32 0 i32 0"))
            val exitCode = process.waitFor()

            if (exitCode == 0) {
                showToast("Disabling SIM card...")
            } else {
                showToast("Command execution failed with exit code $exitCode")
            }
            // Unregister the PhoneStateListener
            runOnUiThread {
                telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_NONE)
            }
        } catch (e: Exception) {
            showToast("Error executing command: ${e.message}")
        }
    }
    private fun adbEnableSIM() {
        try {
            val process = Runtime.getRuntime().exec(arrayOf("su", "shell", "-c", "service call phone 193 i32 0 i32 1"))
            val exitCode = process.waitFor()

            if (exitCode == 0) {
                showToast("Enable SIM card...")
            } else {
                showToast("Command execution failed with exit code $exitCode")
            }
            // Re-register the PhoneStateListener
            runOnUiThread {
                telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_SIGNAL_STRENGTHS)
            }
        } catch (e: Exception) {
            showToast("Error executing command: ${e.message}")
        }
    }
    private fun showToast(message: String) {
        runOnUiThread {
            Toast.makeText(this, message, Toast.LENGTH_SHORT).show()
        }
    }
    @RequiresApi(Build.VERSION_CODES.Q)
    private fun updateSignalStrengthText(signalStrength: SignalStrength, threshold: Int) {
        current_strength = signalStrength.cellSignalStrengths[0].dbm
        is_switch = (signalStrength.cellSignalStrengths[0].dbm + threshold)<0
        val best_sig = measureNeighboringSignalStrength()
        val signalInfo = "Serving Cell: ${signalStrength.cellSignalStrengths[0].dbm} dBm\nMonitoring Cell: ${best_sig.signal} dBm, Pci: ${best_sig.Pci}, Earfcn: ${best_sig.Earfcn}"
        signalStrengthTextView.text = signalInfo
    }
    override fun onDestroy() {
        super.onDestroy()
        telephonyManager.listen(phoneStateListener, PhoneStateListener.LISTEN_NONE)
        unregisterReceiver(wifiReceiver)

    }

// Keyboard
    fun onNumberButtonClick(view: View) {
        if (view is Button) {
            val currentText = enteredNumberTextView.text.toString()
            val buttonText = view.text.toString()
            enteredNumberTextView.text = currentText + buttonText
        }
    }

    fun onBackspaceClick(view: View) {
        val currentText = enteredNumberTextView.text.toString()
        if (currentText.isNotEmpty()) {
            enteredNumberTextView.text = currentText.substring(0, currentText.length - 1)
        }
    }

    fun onCallButtonClick(view: View, localEmerNumber: String?) {
        try{
            Log.d(
                "M911",
                "Call button clicked!"
            )
            val currentWifi = checkAndDisplayWifiSignalStrength()
            var phoneNumber = enteredNumberTextView.text.toString()
            Log.d(
                "M911",
                "Entered number: $phoneNumber"
            )
            if (phoneNumber.isNotEmpty()) {
                // Serving signal strength lower than Threshold
                if(is_switch){
                    if(best_signal > current_strength){
                        // Switch to other cell
                        Log.d("M911", "Disabling SIM...\n")
                        adbDisableSIM()
                    }
                    else{
                        if (currentWifi != null) {
                            // Swtich to WiFi Calling
                            if(currentWifi.level >= 3){
                                if(phoneNumber == "911"){
                                    Log.d("M911", "Fetching local 911 phone number...\n")
                                    phoneNumber = localEmerNumber.toString()
                                }
                            }
                        }
                    }
                }

                Log.d("M911", "Calling: $phoneNumber")

                Log.d(
                    "M911",
                    "Ready to broadcast CALL_INTENT!"
                )
                val dialIntent = Intent(Intent.ACTION_CALL, Uri.parse("tel:$phoneNumber"))

                // Ensure that the device can handle the dial intent
                if (dialIntent.resolveActivity(packageManager) != null) {
                    // Start the dial intent
                    startActivity(dialIntent)
                } else {
                    // Handle the case where there is no activity to handle the dial intent
                    showToast("Cannot find an app to handle the dial action.")
                }
            } else {

                // Handle the case where the phone number is empty
                measureNeighboringSignalStrength()
                showToast("Please enter a valid phone number.")
            }
        } catch (e: SecurityException) {
            // Handle the case where the app doesn't have necessary permissions
            showToast("Permission to make a call is not granted.")
        } catch (e: Exception) {
            // Handle other exceptions
            showToast("Error occurred while dialing the call.")
        }
    }
// End keyboard

// Location functions
    private fun loadCountyPSAPMap(): Map<String, String> {
        val inputStream = assets.open("county_numbers.json")
        val reader = InputStreamReader(inputStream)
        val type = object : TypeToken<Map<String, List<CountyNumber>>>() {}.type
        val countyPSAPList: Map<String, List<CountyNumber>> = Gson().fromJson(reader, type)
        return countyPSAPList["county_numbers"]!!.associate { it.county to it.number }
    }

    private fun getLastLocation() {
        fusedLocationClient.lastLocation
            .addOnSuccessListener { location: Location? ->
                location?.let {
                    getCountyName(it)
                }
            }
    }

    private fun getCountyName(location: Location) {
        val geocoder = Geocoder(this, Locale.getDefault())
        try {
            val addresses: List<Address> =
                geocoder.getFromLocation(location.latitude, location.longitude, 1) as List<Address>
            if (addresses.isNotEmpty()) {
                countyName = addresses[0].subAdminArea
                PSAPNumber = countyPSAPMap[countyName].toString()
                if (countyName != null) {
                    cityNameTextView.text = "Current location: $countyName\nPSAP Number: $PSAPNumber"
                } else {
                    cityNameTextView.text = "Current location: $countyName\nNumber not found!"
                }
            }
        } catch (e: IOException) {
            e.printStackTrace()
        }
    }

    companion object {
        private const val LOCATION_PERMISSION_REQUEST_CODE = 1
    }
// End Location functions


}