#pragma warning disable CS0420

using System.Diagnostics;
using System.Diagnostics.CodeAnalysis;

// A simple read/write lock implementation
class ReadWriteLock
{
    private volatile uint readRC = 0;

    private volatile uint writeRC = 0;

    public bool TryAcquireReadLock([NotNullWhen(true)] out ReadPart? readPart)
    {
        readPart = null;

        if (Volatile.Read(ref writeRC) is 0)
        {
            Interlocked.Increment(ref readRC);

            // Someone acquired the write lock when we adding read lock reference counter
            // We should give it up.
            if (Volatile.Read(ref writeRC) is not 0)
            {
                Interlocked.Decrement(ref readRC);
            }
            else
            {
                readPart = new ReadPart(this);
                return true;
            }
        }

        return false;
    }

    public bool TryAcquireWriteLock([NotNullWhen(true)] out WritePart? writePart)
    {
        writePart = null;

        if (Volatile.Read(ref readRC) is 0
                && Volatile.Read(ref writeRC) is 0)
        {
            Interlocked.Increment(ref writeRC);

            // Another callsite acquired the write lock or a read lock when we acquire write lock
            // We should give it up.
            if (Volatile.Read(ref readRC) != 0 || Volatile.Read(ref writeRC) > 1)
            {
                Interlocked.Decrement(ref writeRC);
            }
            else
            {
                writePart = new WritePart(this);
                return true;
            }
        }

        return false;
    }

    public async Task<ReadPart?> AcquireReadLock(bool blockIfUnavaliable = false)
    {
        while (true)
        {
            if (TryAcquireReadLock(out var readPart))
                return readPart;

            if (blockIfUnavaliable)
                await Task.Yield();
            else
                return null;
        }
    }

    public async Task<WritePart?> AcquireWriteLock(bool blockIfUnavaliable = false)
    {
        while (true)
        {
            if (TryAcquireWriteLock(out var writePart))
                return writePart;

            if (blockIfUnavaliable)
                await Task.Yield();
            else
                return null;
        }
    }

    private void ReleaseReadLock()
    {
        Debug.Assert(Volatile.Read(ref writeRC) is 0);

        Interlocked.Decrement(ref readRC);
    }

    private void ReleaseWriteLock()
    {
        Debug.Assert(Volatile.Read(ref readRC) is 0);

        Interlocked.Decrement(ref writeRC);

        Debug.Assert(Volatile.Read(ref writeRC) is 0);
    }

    public struct ReadPart : IDisposable
    {
        private readonly ReadWriteLock _base = null!;
        private bool disposed = false;

        internal ReadPart(ReadWriteLock @base) => _base = @base;

        public void Dispose() => ReleaseInternal();

        public void Release()
            => ReleaseInternal();

        private void ReleaseInternal()
        {
            if (disposed)
                return;

            disposed = true;
            _base.ReleaseReadLock();
        }
    }

    public struct WritePart : IDisposable
    {
        private readonly ReadWriteLock _base = null!;
        private bool disposed = false;

        internal WritePart(ReadWriteLock @base) => _base = @base;

        public void Dispose() => ReleaseInternal();

        public void Release()
            => ReleaseInternal();

        private void ReleaseInternal()
        {
            if (disposed)
                return;

            disposed = true;
            _base.ReleaseWriteLock();
        }
    }
}