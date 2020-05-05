using System.Collections;
using System.Collections.Generic;
using UnityEngine;
using System;
using System.Net;
using System.Net.Sockets;
using System.Text;
using System.Threading;
using DigitalRuby.RainMaker;

public class UDP_test : MonoBehaviour
{
    public GameObject rain;
    public int int_val = 3;
    byte[] bytes;
    UdpClient listener;
    IPEndPoint groupEP;

    IPAddress broadcast = IPAddress.Parse("127.0.0.1");
    byte[] sendbuf = Encoding.ASCII.GetBytes("RegisterCommand;Rain Intensity;False;Float;0;0.2;1;");

    private const int listenPort = 60002;
    private const int broadcastPort = 60003;
    // Start is called before the first frame update
    void Start()
    {
        listener = new UdpClient(listenPort);
        groupEP = new IPEndPoint(IPAddress.Any, listenPort);
        bytes = null;
        Socket s = new Socket(AddressFamily.InterNetwork, SocketType.Dgram, ProtocolType.Udp);
        IPEndPoint ep = new IPEndPoint(broadcast, broadcastPort);
        s.SendTo(sendbuf, ep);
        Thread t = new Thread(new ThreadStart(udp_listener_thread));
        t.Start();

        //rain.GetComponent<RainScript>().RainIntensity = 0.5f;
    }

    // Update is called once per frame
    void Update()
    {
        //Debug.Log(bytes);
        if (bytes != null)
        {
            string recieved_command = Encoding.ASCII.GetString(bytes, 0, bytes.Length);
            string[] command = recieved_command.Split(';');
            if(command[0] == "Rain Intensity")
            {
                setRainIntensity(float.Parse(command[1]));
            }
            Debug.Log($" {Encoding.ASCII.GetString(bytes, 0, bytes.Length)}");
            //bytes = null;
        }
    }

    void udp_listener_thread()
    {
        try
        {
            while (true)
            {
                //Debug.Log("Working on thread");
                Debug.Log("Waiting for broadcast");
                bytes = listener.Receive(ref groupEP);

                Debug.Log($"Received broadcast from {groupEP} :");
                Debug.Log($" {Encoding.ASCII.GetString(bytes, 0, bytes.Length)}");
            }
        }
        catch (SocketException e)
        {
            Debug.Log(e);
        }
        finally
        {
            listener.Close();
        }
    }
    void udp_broadcast_thread()
    {
    }
    void setRainIntensity(float intensity)
    {
        rain.GetComponent<RainScript>().RainIntensity = intensity;
    }
}
