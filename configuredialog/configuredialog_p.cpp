// configuredialog_p.cpp: classes internal to ConfigureDialog
// see configuredialog.cpp for details.

// my header:
#include "configuredialog_p.h"

// other KMail headers:
#include "settings/globalsettings.h"

// other KDE headers:
#include <QTabWidget>
#include <QDialog>

// Qt headers:
#include <QShowEvent>
#include <QVBoxLayout>

// Other headers:
#include <assert.h>
#include <KConfigGroup>

ConfigModuleWithTabs::ConfigModuleWithTabs(QWidget *parent)
    : ConfigModule(parent), mWasInitialized(false)
{
    QVBoxLayout *vlay = new QVBoxLayout(this);
    vlay->setMargin(0);
    mTabWidget = new QTabWidget(this);
    vlay->addWidget(mTabWidget);
}

void ConfigModuleWithTabs::addTab(ConfigModuleTab *tab, const QString &title)
{
    mTabWidget->addTab(tab, title);
    connect(tab, SIGNAL(changed(bool)),
            this, SIGNAL(changed(bool)));
}

void ConfigModuleWithTabs::showEvent(QShowEvent *event)
{
    mWasInitialized = true;
    ConfigModule::showEvent(event);
}

void ConfigModuleWithTabs::load()
{
    const int numberOfTab = mTabWidget->count();
    for (int i = 0 ; i < numberOfTab ; ++i) {
        ConfigModuleTab *tab = dynamic_cast<ConfigModuleTab *>(mTabWidget->widget(i));
        if (tab) {
            tab->load();
        }
    }
    KCModule::load();
}

void ConfigModuleWithTabs::save()
{
    if (mWasInitialized) {
        KCModule::save();
        const int numberOfTab = mTabWidget->count();
        for (int i = 0 ; i < numberOfTab ; ++i) {
            ConfigModuleTab *tab = dynamic_cast<ConfigModuleTab *>(mTabWidget->widget(i));
            if (tab) {
                tab->save();
            }
        }
    }
}

void ConfigModuleWithTabs::defaults()
{
    ConfigModuleTab *tab = dynamic_cast<ConfigModuleTab *>(mTabWidget->currentWidget());
    if (tab) {
        tab->defaults();
    }
    KCModule::defaults();
}

void ConfigModuleTab::load()
{
    mEmitChanges = false;
    doLoadFromGlobalSettings();
    doLoadOther();
    mEmitChanges = true;
}

void ConfigModuleTab::defaults()
{
    // reset settings which are available via GlobalSettings to their defaults
    // (stolen from KConfigDialogManager::updateWidgetsDefault())
    const bool bUseDefaults = GlobalSettings::self()->useDefaults(true);
    doLoadFromGlobalSettings();
    GlobalSettings::self()->useDefaults(bUseDefaults);
    // reset other settings to default values
    doResetToDefaultsOther();
}

void ConfigModuleTab::slotEmitChanged(void)
{
    if (mEmitChanges) {
        emit changed(true);
    }
}

