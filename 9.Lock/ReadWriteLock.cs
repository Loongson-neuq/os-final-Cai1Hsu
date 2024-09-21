using System.Threading;

// A simple read/write lock for singlethreaded async concurrent scenario
// OS level atomic variable is needed for for multithread scenario
class ReadWriteLock
{
    private volatile nuint readRC = default;

    private volatile bool writeAquired = default;

    public bool AquireReadLock(bool blockIfUnavaliable = false)
    {
        switch ((writeAquired, blockIfUnavaliable))
        {
            case (false, _):
                readRC++;
                return true;
            
            case (true, false):
                return false;

            case (true, true):
                while (writeAquired)
                {
                    // Yield CPU time to the OS
                    Yield();
                }
                readRC++;
                return true;
        }
    }

    public bool AquireWriteLock(bool blockIfUnavaliable = false)
    {
        switch ((readRC == 0, blockIfUnavaliable))
        {
            case (true, _):
                writeAquired = true;
                return true;

            case (false, false):
                return false;

            case (false, true):
                while (writeAquired)
                {
                    // Yield CPU to the OS
                    Yield();
                }
                writeAquired = true;
                return true;
        }
    }

    public void ReleaseReadLock()
    {
        Debug.Assert(!writeAquired);
        readRC--;

        if (readRC < 0)
        {
            throw new Exception("Read lock reference counter is negative! Please check the callsite");
        }
    }

    public void ReleaseWriteLock()
    {
        Debug.Assert(readRC == 0);
        writeAquired = false;
    }
}