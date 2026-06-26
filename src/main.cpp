#include <Geode/Geode.hpp>
#include <Geode/modify/PauseLayer.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

// Almacena el estado original del nivel para restaurarlo al salir
bool g_originalTwoPlayerState = false;
bool g_hasStoredOriginal = false;

class $modify(PlayLayer) {
    // Restaurar el estado original cuando el usuario sale del nivel (destrucción de la capa)
    void onQuit() {
        if (g_hasStoredOriginal && m_levelSettings) {
            m_levelSettings->m_isTwoPlayerMode = g_originalTwoPlayerState;
        }
        g_hasStoredOriginal = false; // Resetear bandera de control
        PlayLayer::onQuit();
    }
};

class $modify(MyPauseLayer, PauseLayer) {
    bool init(bool p0) {
        if (!PauseLayer::init(p0)) return false;

        auto playLayer = PlayLayer::get();
        if (!playLayer || !playLayer->m_levelSettings) return true;

        // Guardar el estado real del nivel una sola vez al iniciar la partida
        if (!g_hasStoredOriginal) {
            g_originalTwoPlayerState = playLayer->m_levelSettings->m_isTwoPlayerMode;
            g_hasStoredOriginal = true;
        }

        // Crear menú e interfaz visual del Toggle
        auto menu = this->getChildByID("right-button-menu"); // Ubicación en el lateral derecho de la pausa
        if (!menu) return true;

        auto sprOff = CCSprite::createWithSpriteFrameName("GJ_checkOff_001.png");
        auto sprOn = CCSprite::createWithSpriteFrameName("GJ_checkOn_001.png");

        auto toggler = CCMenuItemToggler::create(
            sprOff, sprOn, this, menu_selector(MyPauseLayer::onToggleTwoPlayer)
        );
        
        // El estado inicial del Check se adapta dinámicamente al nivel activo
        toggler->toggle(playLayer->m_levelSettings->m_isTwoPlayerMode);
        menu->addChild(toggler);
        menu->updateLayout();

        return true;
    }

    void onToggleTwoPlayer(CCObject* sender) {
        auto playLayer = PlayLayer::get();
        if (playLayer && playLayer->m_levelSettings) {
            auto toggler = static_cast<CCMenuItemToggler*>(sender);
            // Modifica el comportamiento en tiempo real de la partida sin lanzar notificaciones
            playLayer->m_levelSettings->m_isTwoPlayerMode = !toggler->isToggled();
        }
    }
};
