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
                LAUNCH,
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
                OBJECT_MOVE = BaseCommand::LAST,
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
                LOAD_SHADER = BaseCommand::LAST,
            };
    };
}

#endif // NODE_INFO_HPP
