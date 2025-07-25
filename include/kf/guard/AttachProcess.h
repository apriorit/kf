#pragma once

namespace kf
{
    class AttachProcess
    {
    public:
        explicit AttachProcess(PEPROCESS process)
        {
            KeStackAttachProcess(process, &m_apcState);
        }

        ~AttachProcess()
        {
            KeUnstackDetachProcess(&m_apcState);
        }

    private:
        KAPC_STATE m_apcState;
    };
}
