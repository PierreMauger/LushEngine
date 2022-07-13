#ifndef NODE_INFO_HPP
#define NODE_INFO_HPP

namespace Lush
{
    enum class Module {
        AUDIO,
        CORE,
        INPUT,
        LOADER,
        RENDER,

        BROADCAST = -1
    };

    class BaseCommand
    {
        public:
            enum {
                QUIT,
                SLEEP_FOR,
                LAST
            };
    };

    class AudioCommand : public BaseCommand
    {
        public:
            enum {
                LAST = BaseCommand::LAST,
            };
    };

    class CoreCommand : public BaseCommand
    {
        public:
            enum {
                LAST = BaseCommand::LAST,
            };
    };

    class InputCommand : public BaseCommand
    {
        public:
            enum {
                LAST = BaseCommand::LAST,
            };
    };

    class LoaderCommand : public BaseCommand
    {
        public:
            enum {
                LAST = BaseCommand::LAST,
            };
    };

    class RenderCommand : public BaseCommand
    {
        public:
            enum {
                LAST = BaseCommand::LAST,
            };
    };
}

#endif // NODE_INFO_HPP
