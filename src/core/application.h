#pragma once
#include <memory>
#include <core/games_lists.h>
#include <video/graphics_types.h>
#include <generic_kernel/types/kprocess.h>

#include <core/process_qol.h>
namespace Plusnx::Os {
    class NxSys;
};

namespace Plusnx::Core {
    class Context;

    class Application {
    public:
        Application();
        ~Application();
        void Initialize(std::shared_ptr<Video::GraphicsSupportContext>&& support);
        void LoadAGameByIndex(u64 index = 0) const;

        // Only selects a game from the collection (does not load it yet)
        bool PickByName(const std::string& game);
        // Convert various file formats into a GameFS
        bool ExtractIntoGameFs();
        void SaveUserInformation() const;

        void ClearUiEvents() const;
        void UpdateFrame() const;

        std::shared_ptr<Context> context;

        std::unique_ptr<ProcessQol> appQol;
    private:
        SysFs::SysPath declared;
        SysFs::SysPath chosen;

        std::shared_ptr<Video::GraphicsSupportContext> ui;
        std::unique_ptr<GamesLists> games;
    };
}
