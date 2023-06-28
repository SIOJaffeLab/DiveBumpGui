#include "Interface.h"

Interface::Interface(){
    cout << "\nCurrent Configuration:" << endl;
    selfpath = get_selfpath();
    cout << "Executable path: " + selfpath + "\n" << endl;
    configRead(selfpath+"/config/interface_config");
    cout << endl;
}

Interface::~Interface(){}

//Reads interface configuration file
//Creates the device objects
char Interface::Setup()
{
    vn100 = new VN100(selfpath+"/config/vn100_config");
    cout << endl;
    sbe39 = new SBE39(selfpath+"/config/sbe39_config");
    cout << endl;
    uuv = new UUV(selfpath+"/config/uuv_config");
    cout << endl; 
    batt1 = new Battery(selfpath+"/config/batt1_config");
    cout << endl;
    pa200 = new PA200(selfpath+"/config/pa200_config");
    cout << endl;

    return 1;
}

//Starts the interface loop that waits for a killchar on stdin to end
//Loop time based on value written in config file
//Returns true once the loop has ended
char Interface::Loop()
{
    while (true)
    {
        cout << "\nEnter 'q' to stop the program and return to the top-menu" << endl;
        string str;
        int ret = 0;

        while (true)
        {
            //Performs non-blocking reads of stdin for all program commands
            ret = cmdInterpreter();
            if ((ret == -1))
            {
                break;
            }

            //Gets the most recent data from devices
            getData();

            //Sleeps for loop_time milliseconds
            usleep((loop_time)*1000);
        }

        vn100->rawData.clear();
        sbe39->rawData.clear();
        batt1->rawData.clear();
        pa200->rawData.clear();
        uuv->rawData.clear();
    }



    return 1;
}

// Create the data file directory and start the device threads
char Interface::Start()
{
    string foldername = createFolder();
    sbe39->Start(foldername);
    batt1->Start(foldername);
    vn100->Start(foldername);
    pa200->Start(foldername);
    uuv->Start(foldername);
    sleep(1);

    Loop();

    return 1;
}

//Performs non-blocking reads of stdin and interprets commands
char Interface::cmdInterpreter()
{
    string cmd;

    if (input_wait() == 1)
    {
        // Listen for user command to stop and return to top menu
        getline(cin, cmd);
        cmd.erase( cmd.find_last_not_of( " \f\n\r\t\v" ) + 1 );
        if (cmd.compare("q") == 0)
        {
            cout << endl;
            vn100->Stop();
            sbe39->Stop();
            batt1->Stop();
            pa200->Stop();
            uuv->Stop();
            Menu();
            return -1;
        }
        else
        {
            //cout << "\nCommand not recognized: Enter 'q' to stop program and return to top-menu" << endl;
            cout << cmd << endl;
            uuv->sendCommand(cmd);
            return -2;
        }

    }

    return 0;
}

//Displays most recent data from devices
//Nothing is outputted if no new data was grabbed from the device
char Interface::getData()
{
    if ((vn100->bNew || uuv->bNew || sbe39->bNew || batt1->bNew || pa200->bNew))
    {
        cout << endl;
        if (vn100->bNew)
            cout << vn100->rawData;
        if (sbe39->bNew)
            cout << sbe39->rawData;
        if (batt1->bNew)
            cout << batt1->rawData;
        if (pa200->bNew)
            cout << pa200->rawData;
        if (uuv->bNew)
            cout << uuv->rawData;
    }

    vn100->bNew = false;
    sbe39->bNew = false;
    batt1->bNew = false;
    pa200->bNew = false;
    uuv->bNew = false;

    return 1;
}


// Function to wait until user input is available
char Interface::input_wait()
{
    fd_set set;
    struct timeval tv;
    tv.tv_sec = 0;
    tv.tv_usec = 0;

    FD_ZERO (&set);
    FD_SET (STDIN_FILENO, &set);

    return (select (FD_SETSIZE, &set,NULL, NULL, &tv));
}

string Interface::createFolder()
{
    string foldername = selfpath + "/Data/TowData_" + timestamp(time(0));
    mkdir(foldername.c_str(), 0777);

    return foldername;
}

//Returns the timestamp string used to create unique file and folder names
string Interface::timestamp(long timestamp)
{
    //Gets current time since epoch
    const time_t rawtime = (const time_t)timestamp;

    struct tm * dt;
    char timestr[30];
    char buffer [30];

    //Converts rawtime into local time
    dt = localtime(&rawtime);

    //Formats time to be human readable
    strftime(timestr, sizeof(timestr), "%m-%d-%Y_%H-%M-%S", dt);
    sprintf(buffer,"%s", timestr);
    std::string stdBuffer(buffer);

    //Return the timestamp string
    return stdBuffer;

}

char Interface::configRead(string config_path)
{
    string buf;
    ifstream file;
    int i;
    file.open(config_path.c_str());

    if (file.is_open())
    {
        while ( file.good() )
        {
            getline (file, buf);
            if ((sscanf(buf.c_str(), "LOOP_TIME=%d", &i)) == 1)
            {
                loop_time = i;
                cout << "Loop time: " << loop_time << endl;;
            }
        }
        file.close();
    }
    else
        cout << "Error opening file! Make sure config file path is correctly set" << endl;
    return 1;
}

char Interface::Menu()
{
    char input[50];

    while (true)
    {
        bool bMenu = true;
        string selfpath = get_selfpath();
        cout << "\n1. Start Program\n2. Change settings\n3. Exit\n\n";
        getInput(input);

        if (input[0] == '1')
        {
            Start();
        }
        else if (input[0] == '2')
        {

            while (bMenu)
            {
                cout << "1. Interface\n2. VN100\n3. SBE39\n4. BATT1\n5. PA200\n6. Main Menu\n";
                getInput(input);

                if (input[0] == '1')
                {
                    interfaceConfig(selfpath+"/config/interface_config");
                }
                else if (input[0] == '2')
                {
                    deviceConfig(vn100, selfpath+"/config/vn100_config", true);
                }
                else if (input[0] == '3')
                {
                    deviceConfig(sbe39, selfpath+"/config/sbe39_config", false);
                }
                else if (input[0] == '4')
                {
                    deviceConfig(batt1, selfpath+"/config/batt1_config", false);
                }
                else if (input[0]=='5')
                {
                    deviceConfig(pa200, selfpath+"/config/pa200_config", false);
                }
                else if (input[0]=='6')
                {
                    bMenu=false;
                }
            }
        }
        else if (input[0]=='3')
        {
            exit(1);
        }
    }
}

char Interface::getInput(char input[])
{
    memset(&input[0], 0, sizeof(input));
    string s;
    s.clear();
    getline(cin, s);
    if (s.size() <= 0)
    {
        return -1;
    }
    else if (s.size() > 50)
    {
        cout << "Invalid input! Input length exceeded maximum characters." << endl;
        return -2;
    }
    else
    {
        s.erase( s.find_last_not_of( " \f\n\r\t\v" ) + 1 );
        stringstream ss(s);
        ss >> input;
        return 1;
    }
    //s.clear();
}

char Interface::intConvert(char value[], int range_min, int range_max, int &result)
{
    int i;
    string s(value);
    //cout << s << endl;
    stringstream ss(s);
    ss >> i;
    //cout << i << endl;
    if (!ss)
    {
        cout << "You entered a non-integer value. Setting not changed." << endl;
        return -1;
    }
    else if ((i >= range_min) && (i <= range_max))
    {
        result = i;
        //cout << result << endl;
        return 1;
    }
    else
        cout << "Input was out of range. Min: " << range_min << " Max: " << range_max << endl;
    return -2;
}

char Interface::deviceConfig(Device *device, string config_path, bool trim = false)
{
    char input[50];
    int i;
    ofstream outfile;
    char writebuf[1000];
    int strlen;

    //NAME config parameter
    cout << "Device Name: " << device->name << ": ";
    if ((getInput(input) == 1))
    {
        //cout << input << endl;
        device->name = input;
    }
    cout << device->name << endl;

    //PATH config parameter
    cout << "Serial Port Path: " << device->comPort << ": ";
    if (getInput(input)==1)
    {
        //cout << input[0] << endl;
        device->comPort = input;
    }

    //BAUD config parameter
    cout << "Baud Rate: " << device->baud << ": ";
    if ((getInput(input)==1) && (intConvert(input, 0, 1000000, i)==1))
    {
        //cout << i << endl;
        device->baud = i;
    }

    //DISPLAY_DATA config parameter
    cout << "Display Data (Y or N): " << device->display << ": ";
    if ((getInput(input)==1))
    {
        if ((strcmp(input, "Y") == 0))
        {
            device->bDisplay = true;
            device->display = "Y";
        }
        else
        {
            device->bDisplay = false;
            device->display = "N";
        }
        //cout << display << endl;

    }
    //TRIM_DATA config parameter
    if (trim)
    {
        cout << "Trim Data (Y or N): " << device->trim << ": ";
        if ((getInput(input)==1))
        {
            if ((strcmp(input, "Y") == 0))
            {
                device->bTrim = true;
                device->trim = "Y";
            }
            else
            {
                device->bTrim = false;
                device->trim = "N";
            }
            //cout << trim << endl;
        }
    }

    //Open configuration file and delete its contents
    //Fill a temporary buffer (writebuf) with parameters to write
    //Write parameters to the config file (unchanged parameters are still rewritten)
    //Close the config file
    outfile.open(config_path.c_str(), ofstream::out | ofstream::trunc);
    if (trim)
        strlen = sprintf(writebuf, "NAME=%s\nPATH=%s\nBAUD=%d\nDISPLAY_DATA=%s\nTRIM_DATA=%s\n", device->name.c_str(), device->comPort.c_str(), device->baud, device->display.c_str(), device->trim.c_str());
    else
        strlen = sprintf(writebuf, "NAME=%s\nPATH=%s\nBAUD=%d\nDISPLAY_DATA=%s\n", device->name.c_str(), device->comPort.c_str(), device->baud, device->display.c_str());
    cout << writebuf << endl;
    outfile.write(writebuf, strlen);
    outfile.close();

    return 1;
}

char Interface::interfaceConfig(string config_path)
{
    char input[50];
    char writebuf[1000];
    int i;
    int strlen;
    ofstream outfile;

    //LOOP_TIME config parameter
    cout << "Loop time(milliseconds): " << loop_time << ": ";
    if ((getInput(input) == 1) && (intConvert(input, 0, 5000, i)==1))
    {
        loop_time = i;
        //cout << "Condition passed" << endl;
    }
    cout << loop_time << endl;

    //Open configuration file and delete its contents
    //Fill a temporary buffer (writebuf) with parameters to write
    //Write parameters to the config file (unchanged parameters are still rewritten)
    //Close the config file
    outfile.open(config_path.c_str(), ofstream::out | ofstream::trunc);
    strlen = sprintf(writebuf, "LOOP_TIME=%d\n", loop_time);
    cout << writebuf;
    outfile.write(writebuf, strlen);
    outfile.close();

    return 1;
}

string Interface::get_selfpath() {
    string selfpath;
    char buff[1024];
    ssize_t len = ::readlink("/proc/self/exe", buff, sizeof(buff)-1);
    if (len != -1) {
        buff[len] = '\0';
        selfpath = string(buff);
        for (string::iterator it = selfpath.end(); it >= selfpath.begin(); it--){
            if (*it =='/'){
                selfpath.erase(it, selfpath.end());
                break;
            }
        }
        return selfpath;
    } else {
        cout << "Could not locate executable path." << endl;
    }
    return 0;
}
