Import("env")
import serial
import time
import serial.tools.list_ports

def get_upload_port(env):
    """Ermittelt den Upload-Port"""
    port = env.subst("$UPLOAD_PORT")
    
    if not port or port == "":
        ports = list(serial.tools.list_ports.comports())
        if ports:
            port = ports[0].device
            print(f"Auto-detected port: {port}")
        else:
            print("⚠️ No COM port found!")
            return None
    
    return port

def custom_reset_sequence(source, target, env):
    port = get_upload_port(env)
    if not port:
        return
    
    print(f"🔄 ISP Boot Sequence on {port}...")
    
    try:
        ser = serial.Serial(port, 115200, timeout=1)
        
        ser.setDTR(False)  # DTR LOW = Reset
        ser.setRTS(True)   # RTS HIGH = Boot0
        time.sleep(0.1)
        
        ser.setDTR(True)   # Reset freigeben
        time.sleep(0.05)
        
        ser.close()
        print("✅ Reset sequence complete!")
        
    except Exception as e:
        print(f"❌ Error: {e}")

def reset_after_upload(source, target, env):
    port = get_upload_port(env)
    if not port:
        return
    
    print(f"🔄 Resetting device on {port}...")
    
    try:
        ser = serial.Serial(port, 115200, timeout=1)
        
        ser.setDTR(False)  # DTR LOW = Reset
        ser.setRTS(False)  # RTS LOW = Normal Boot
        time.sleep(0.1)
        
        ser.setDTR(True)   # Reset freigeben
        
        ser.close()
        print("✅ Device reset complete!")
        
    except Exception as e:
        print(f"❌ Error: {e}")

env.AddPreAction("upload", custom_reset_sequence)
env.AddPostAction("upload", reset_after_upload)