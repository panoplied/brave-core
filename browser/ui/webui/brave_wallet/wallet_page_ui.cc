/* Copyright (c) 2021 The Brave Authors. All rights reserved.
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "brave/browser/ui/webui/brave_wallet/wallet_page_ui.h"

#include <utility>

#include "base/files/file_path.h"
#include "brave/browser/brave_wallet/asset_ratio_controller_factory.h"
#include "brave/browser/brave_wallet/brave_wallet_service_factory.h"
#include "brave/browser/brave_wallet/eth_tx_controller_factory.h"
#include "brave/browser/brave_wallet/keyring_controller_factory.h"
#include "brave/browser/brave_wallet/rpc_controller_factory.h"
#include "brave/browser/brave_wallet/swap_controller_factory.h"
#include "brave/browser/ui/webui/brave_wallet/wallet_common_ui.h"
#include "brave/browser/ui/webui/navigation_bar_data_provider.h"
#include "brave/common/webui_url_constants.h"
#include "brave/components/brave_wallet/browser/asset_ratio_controller.h"
#include "brave/components/brave_wallet/browser/brave_wallet_constants.h"
#include "brave/components/brave_wallet/browser/brave_wallet_service.h"
#include "brave/components/brave_wallet/browser/erc_token_registry.h"
#include "brave/components/brave_wallet/browser/eth_json_rpc_controller.h"
#include "brave/components/brave_wallet/browser/eth_tx_controller.h"
#include "brave/components/brave_wallet/browser/keyring_controller.h"
#include "brave/components/brave_wallet/browser/swap_controller.h"
#include "brave/components/brave_wallet_page/resources/grit/brave_wallet_page_generated_map.h"
#include "chrome/browser/profiles/profile.h"
#include "chrome/browser/ui/webui/webui_util.h"
#include "components/grit/brave_components_resources.h"
#include "components/grit/brave_components_strings.h"
#include "content/public/browser/web_contents.h"
#include "content/public/browser/web_ui.h"
#include "content/public/browser/web_ui_data_source.h"
#include "ui/base/accelerators/accelerator.h"
#include "ui/base/webui/web_ui_util.h"

WalletPageUI::WalletPageUI(content::WebUI* web_ui)
    : ui::MojoWebUIController(web_ui,
                              true /* Needed for webui browser tests */) {
  content::WebUIDataSource* source =
      content::WebUIDataSource::Create(kWalletPageHost);

  static constexpr webui::LocalizedString kStrings[] = {
      {"braveWallet", IDS_BRAVE_WALLET},
      { "braveWalletDefiCategory", IDS_BRAVE_WALLET_DEFI_CATEGORY },
      { "braveWalletNftCategory", IDS_BRAVE_WALLET_NFT_CATEGORY },
      { "braveWalletSearchCategory", IDS_BRAVE_WALLET_SEARCH_CATEGORY },
      { "braveWalletDefiButtonText", IDS_BRAVE_WALLET_DEFI_BUTTON_TEXT },
      { "braveWalletNftButtonText", IDS_BRAVE_WALLET_NFT_BUTTON_TEXT },
      { "braveWalletCompoundName", IDS_BRAVE_WALLET_COMPOUND_NAME },
      { "braveWalletCompoundDescription", IDS_BRAVE_WALLET_COMPOUND_DESCRIPTION },
      { "braveWalletMakerName", IDS_BRAVE_WALLET_MAKER_NAME },
      { "braveWalletMakerDescription", IDS_BRAVE_WALLET_MAKER_DESCRIPTION },
      { "braveWalletAaveName", IDS_BRAVE_WALLET_AAVE_NAME },
      { "braveWalletAaveDescription", IDS_BRAVE_WALLET_AAVE_DESCRIPTION },
      { "braveWalletOpenSeaName", IDS_BRAVE_WALLET_OPEN_SEA_NAME },
      { "braveWalletOpenSeaDescription", IDS_BRAVE_WALLET_OPEN_SEA_DESCRIPTION },
      { "braveWalletRaribleName", IDS_BRAVE_WALLET_RARIBLE_NAME },
      { "braveWalletRaribleDescription", IDS_BRAVE_WALLET_RARIBLE_DESCRIPTION },
      { "braveWalletSearchText", IDS_BRAVE_WALLET_SEARCH_TEXT },
      { "braveWalletSideNavCrypto", IDS_BRAVE_WALLET_SIDE_NAV_CRYPTO },
      { "braveWalletSideNavRewards", IDS_BRAVE_WALLET_SIDE_NAV_REWARDS },
      { "braveWalletSideNavCards", IDS_BRAVE_WALLET_SIDE_NAV_CARDS },
      { "braveWalletTopNavPortfolio", IDS_BRAVE_WALLET_TOP_NAV_PORTFOLIO },
      { "braveWalletTopTabPrices", IDS_BRAVE_WALLET_TOP_TAB_PRICES },
      { "braveWalletTopTabApps", IDS_BRAVE_WALLET_TOP_TAB_APPS },
      { "braveWalletTopNavNFTS", IDS_BRAVE_WALLET_TOP_NAV_N_F_T_S },
      { "braveWalletTopNavAccounts", IDS_BRAVE_WALLET_TOP_NAV_ACCOUNTS },
      { "braveWalletChartLive", IDS_BRAVE_WALLET_CHART_LIVE },
      { "braveWalletChartOneDay", IDS_BRAVE_WALLET_CHART_ONE_DAY },
      { "braveWalletChartOneWeek", IDS_BRAVE_WALLET_CHART_ONE_WEEK },
      { "braveWalletChartOneMonth", IDS_BRAVE_WALLET_CHART_ONE_MONTH },
      { "braveWalletChartThreeMonths", IDS_BRAVE_WALLET_CHART_THREE_MONTHS },
      { "braveWalletChartOneYear", IDS_BRAVE_WALLET_CHART_ONE_YEAR },
      { "braveWalletChartAllTime", IDS_BRAVE_WALLET_CHART_ALL_TIME },
      { "braveWalletAddCoin", IDS_BRAVE_WALLET_ADD_COIN },
      { "braveWalletBalance", IDS_BRAVE_WALLET_BALANCE },
      { "braveWalletAccounts", IDS_BRAVE_WALLET_ACCOUNTS },
      { "braveWalletAccount", IDS_BRAVE_WALLET_ACCOUNT },
      { "braveWalletTransactions", IDS_BRAVE_WALLET_TRANSACTIONS },
      { "braveWalletPrice", IDS_BRAVE_WALLET_PRICE },
      { "braveWalletBack", IDS_BRAVE_WALLET_BACK },
      { "braveWalletAddAccount", IDS_BRAVE_WALLET_ADD_ACCOUNT },
      { "braveWalletBuy", IDS_BRAVE_WALLET_BUY },
      { "braveWalletSend", IDS_BRAVE_WALLET_SEND },
      { "braveWalletSwap", IDS_BRAVE_WALLET_SWAP },
      { "braveWalletBssToolTip", IDS_BRAVE_WALLET_BSS_TOOL_TIP },
      { "braveWalletButtonContinue", IDS_BRAVE_WALLET_BUTTON_CONTINUE },
      { "braveWalletButtonCopy", IDS_BRAVE_WALLET_BUTTON_COPY },
      { "braveWalletButtonVerify", IDS_BRAVE_WALLET_BUTTON_VERIFY },
      { "braveWalletWelcomeTitle", IDS_BRAVE_WALLET_WELCOME_TITLE },
      { "braveWalletWelcomeDescription", IDS_BRAVE_WALLET_WELCOME_DESCRIPTION },
      { "braveWalletWelcomeButton", IDS_BRAVE_WALLET_WELCOME_BUTTON },
      { "braveWalletWelcomeRestoreButton", IDS_BRAVE_WALLET_WELCOME_RESTORE_BUTTON },
      { "braveWalletBackupIntroTitle", IDS_BRAVE_WALLET_BACKUP_INTRO_TITLE },
      { "braveWalletBackupIntroDescription", IDS_BRAVE_WALLET_BACKUP_INTRO_DESCRIPTION },
      { "braveWalletBackupIntroTerms", IDS_BRAVE_WALLET_BACKUP_INTRO_TERMS },
      { "braveWalletBackupButtonSkip", IDS_BRAVE_WALLET_BACKUP_BUTTON_SKIP },
      { "braveWalletBackupButtonCancel", IDS_BRAVE_WALLET_BACKUP_BUTTON_CANCEL },
      { "braveWalletRecoveryTitle", IDS_BRAVE_WALLET_RECOVERY_TITLE },
      { "braveWalletRecoveryDescription", IDS_BRAVE_WALLET_RECOVERY_DESCRIPTION },
      { "braveWalletRecoveryWarning1", IDS_BRAVE_WALLET_RECOVERY_WARNING1 },
      { "braveWalletRecoveryWarning2", IDS_BRAVE_WALLET_RECOVERY_WARNING2 },
      { "braveWalletRecoveryWarning3", IDS_BRAVE_WALLET_RECOVERY_WARNING3 },
      { "braveWalletRecoveryTerms", IDS_BRAVE_WALLET_RECOVERY_TERMS },
      { "braveWalletVerifyRecoveryTitle", IDS_BRAVE_WALLET_VERIFY_RECOVERY_TITLE },
      { "braveWalletVerifyRecoveryDescription", IDS_BRAVE_WALLET_VERIFY_RECOVERY_DESCRIPTION },
      { "braveWalletVerifyError", IDS_BRAVE_WALLET_VERIFY_ERROR },
      { "braveWalletCreatePasswordTitle", IDS_BRAVE_WALLET_CREATE_PASSWORD_TITLE },
      { "braveWalletCreatePasswordDescription", IDS_BRAVE_WALLET_CREATE_PASSWORD_DESCRIPTION },
      { "braveWalletCreatePasswordInput", IDS_BRAVE_WALLET_CREATE_PASSWORD_INPUT },
      { "braveWalletConfirmPasswordInput", IDS_BRAVE_WALLET_CONFIRM_PASSWORD_INPUT },
      { "braveWalletCreatePasswordError", IDS_BRAVE_WALLET_CREATE_PASSWORD_ERROR },
      { "braveWalletConfirmPasswordError", IDS_BRAVE_WALLET_CONFIRM_PASSWORD_ERROR },
      { "braveWalletLockScreenTitle", IDS_BRAVE_WALLET_LOCK_SCREEN_TITLE },
      { "braveWalletLockScreenButton", IDS_BRAVE_WALLET_LOCK_SCREEN_BUTTON },
      { "braveWalletLockScreenError", IDS_BRAVE_WALLET_LOCK_SCREEN_ERROR },
      { "braveWalletWalletPopupSettings", IDS_BRAVE_WALLET_WALLET_POPUP_SETTINGS },
      { "braveWalletWalletPopupLock", IDS_BRAVE_WALLET_WALLET_POPUP_LOCK },
      { "braveWalletWalletPopupBackup", IDS_BRAVE_WALLET_WALLET_POPUP_BACKUP },
      { "braveWalletBackupWarningText", IDS_BRAVE_WALLET_BACKUP_WARNING_TEXT },
      { "braveWalletBackupButton", IDS_BRAVE_WALLET_BACKUP_BUTTON },
      { "braveWalletDismissButton", IDS_BRAVE_WALLET_DISMISS_BUTTON },
      { "braveWalletDefaultWalletBanner", IDS_BRAVE_WALLET_DEFAULT_WALLET_BANNER },
      { "braveWalletRestoreTite", IDS_BRAVE_WALLET_RESTORE_TITE },
      { "braveWalletRestoreDescription", IDS_BRAVE_WALLET_RESTORE_DESCRIPTION },
      { "braveWalletRestoreError", IDS_BRAVE_WALLET_RESTORE_ERROR },
      { "braveWalletRestorePlaceholder", IDS_BRAVE_WALLET_RESTORE_PLACEHOLDER },
      { "braveWalletRestoreShowPhrase", IDS_BRAVE_WALLET_RESTORE_SHOW_PHRASE },
      { "braveWalletRestoreLegacyCheckBox", IDS_BRAVE_WALLET_RESTORE_LEGACY_CHECK_BOX },
      { "braveWalletRestoreFormText", IDS_BRAVE_WALLET_RESTORE_FORM_TEXT },
      { "braveWalletToolTipCopyToClipboard", IDS_BRAVE_WALLET_TOOL_TIP_COPY_TO_CLIPBOARD },
      { "braveWalletAccountsPrimary", IDS_BRAVE_WALLET_ACCOUNTS_PRIMARY },
      { "braveWalletAccountsSecondary", IDS_BRAVE_WALLET_ACCOUNTS_SECONDARY },
      { "braveWalletAccountsSecondaryDisclaimer", IDS_BRAVE_WALLET_ACCOUNTS_SECONDARY_DISCLAIMER },
      { "braveWalletAccountsAssets", IDS_BRAVE_WALLET_ACCOUNTS_ASSETS },
      { "braveWalletAccountsEditVisibleAssets", IDS_BRAVE_WALLET_ACCOUNTS_EDIT_VISIBLE_ASSETS },
      { "braveWalletAddAccountCreate", IDS_BRAVE_WALLET_ADD_ACCOUNT_CREATE },
      { "braveWalletAddAccountImport", IDS_BRAVE_WALLET_ADD_ACCOUNT_IMPORT },
      { "braveWalletAddAccountHardware", IDS_BRAVE_WALLET_ADD_ACCOUNT_HARDWARE },
      { "braveWalletAddAccountConnect", IDS_BRAVE_WALLET_ADD_ACCOUNT_CONNECT },
      { "braveWalletAddAccountPlaceholder", IDS_BRAVE_WALLET_ADD_ACCOUNT_PLACEHOLDER },
      { "braveWalletImportAccountDisclaimer", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_DISCLAIMER },
      { "braveWalletImportAccountPlaceholder", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_PLACEHOLDER },
      { "braveWalletImportAccountKey", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_KEY },
      { "braveWalletImportAccountFile", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_FILE },
      { "braveWalletImportAccountUploadButton", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_UPLOAD_BUTTON },
      { "braveWalletImportAccountUploadPlaceholder", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_UPLOAD_PLACEHOLDER },
      { "braveWalletImportAccountError", IDS_BRAVE_WALLET_IMPORT_ACCOUNT_ERROR },
      { "braveWalletConnectHardwareTitle", IDS_BRAVE_WALLET_CONNECT_HARDWARE_TITLE },
      { "braveWalletConnectHardwareInfo1", IDS_BRAVE_WALLET_CONNECT_HARDWARE_INFO1 },
      { "braveWalletConnectHardwareInfo2", IDS_BRAVE_WALLET_CONNECT_HARDWARE_INFO2 },
      { "braveWalletConnectHardwareInfo3", IDS_BRAVE_WALLET_CONNECT_HARDWARE_INFO3 },
      { "braveWalletConnectHardwareTrezor", IDS_BRAVE_WALLET_CONNECT_HARDWARE_TREZOR },
      { "braveWalletConnectHardwareLedger", IDS_BRAVE_WALLET_CONNECT_HARDWARE_LEDGER },
      { "braveWalletConnectingHardwareWallet", IDS_BRAVE_WALLET_CONNECTING_HARDWARE_WALLET },
      { "braveWalletAddCheckedAccountsHardwareWallet", IDS_BRAVE_WALLET_ADD_CHECKED_ACCOUNTS_HARDWARE_WALLET },
      { "braveWalletLoadMoreAccountsHardwareWallet", IDS_BRAVE_WALLET_LOAD_MORE_ACCOUNTS_HARDWARE_WALLET },
      { "braveWalletSearchScannedAccounts", IDS_BRAVE_WALLET_SEARCH_SCANNED_ACCOUNTS },
      { "braveWalletSwitchHDPathTextHardwareWallet", IDS_BRAVE_WALLET_SWITCH_H_D_PATH_TEXT_HARDWARE_WALLET },
      { "braveWalletLedgerLiveDerivationPath", IDS_BRAVE_WALLET_LEDGER_LIVE_DERIVATION_PATH },
      { "braveWalletLedgerLegacyDerivationPath", IDS_BRAVE_WALLET_LEDGER_LEGACY_DERIVATION_PATH },
      { "braveWalletUnknownInternalError", IDS_BRAVE_WALLET_UNKNOWN_INTERNAL_ERROR },
      { "braveWalletAccountSettingsDetails", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_DETAILS },
      { "braveWalletAccountSettingsWatchlist", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_WATCHLIST },
      { "braveWalletAccountSettingsPrivateKey", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_PRIVATE_KEY },
      { "braveWalletAccountSettingsSave", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_SAVE },
      { "braveWalletAccountSettingsRemove", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_REMOVE },
      { "braveWalletWatchlistAddCustomAsset", IDS_BRAVE_WALLET_WATCHLIST_ADD_CUSTOM_ASSET },
      { "braveWalletWatchListTokenName", IDS_BRAVE_WALLET_WATCH_LIST_TOKEN_NAME },
      { "braveWalletWatchListTokenAddress", IDS_BRAVE_WALLET_WATCH_LIST_TOKEN_ADDRESS },
      { "braveWalletWatchListTokenSymbol", IDS_BRAVE_WALLET_WATCH_LIST_TOKEN_SYMBOL },
      { "braveWalletWatchListTokenDecimals", IDS_BRAVE_WALLET_WATCH_LIST_TOKEN_DECIMALS },
      { "braveWalletWatchListAdd", IDS_BRAVE_WALLET_WATCH_LIST_ADD },
      { "braveWalletWatchListSuggestion", IDS_BRAVE_WALLET_WATCH_LIST_SUGGESTION },
      { "braveWalletWatchListNoAsset", IDS_BRAVE_WALLET_WATCH_LIST_NO_ASSET },
      { "braveWalletWatchListSearchPlaceholder", IDS_BRAVE_WALLET_WATCH_LIST_SEARCH_PLACEHOLDER },
      { "braveWalletWatchListError", IDS_BRAVE_WALLET_WATCH_LIST_ERROR },
      { "braveWalletAccountSettingsDisclaimer", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_DISCLAIMER },
      { "braveWalletAccountSettingsShowKey", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_SHOW_KEY },
      { "braveWalletAccountSettingsHideKey", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_HIDE_KEY },
      { "braveWalletAccountSettingsUpdateError", IDS_BRAVE_WALLET_ACCOUNT_SETTINGS_UPDATE_ERROR },
      { "braveWalletPreset25", IDS_BRAVE_WALLET_PRESET25 },
      { "braveWalletPreset50", IDS_BRAVE_WALLET_PRESET50 },
      { "braveWalletPreset75", IDS_BRAVE_WALLET_PRESET75 },
      { "braveWalletPreset100", IDS_BRAVE_WALLET_PRESET100 },
      { "braveWalletNetworkETH", IDS_BRAVE_WALLET_NETWORK_E_T_H },
      { "braveWalletNetworkMain", IDS_BRAVE_WALLET_NETWORK_MAIN },
      { "braveWalletNetworkTest", IDS_BRAVE_WALLET_NETWORK_TEST },
      { "braveWalletNetworkRopsten", IDS_BRAVE_WALLET_NETWORK_ROPSTEN },
      { "braveWalletNetworkKovan", IDS_BRAVE_WALLET_NETWORK_KOVAN },
      { "braveWalletNetworkRinkeby", IDS_BRAVE_WALLET_NETWORK_RINKEBY },
      { "braveWalletNetworkGoerli", IDS_BRAVE_WALLET_NETWORK_GOERLI },
      { "braveWalletNetworkBinance", IDS_BRAVE_WALLET_NETWORK_BINANCE },
      { "braveWalletNetworkBinanceAbbr", IDS_BRAVE_WALLET_NETWORK_BINANCE_ABBR },
      { "braveWalletNetworkLocalhost", IDS_BRAVE_WALLET_NETWORK_LOCALHOST },
      { "braveWalletSelectAccount", IDS_BRAVE_WALLET_SELECT_ACCOUNT },
      { "braveWalletSearchAccount", IDS_BRAVE_WALLET_SEARCH_ACCOUNT },
      { "braveWalletSelectNetwork", IDS_BRAVE_WALLET_SELECT_NETWORK },
      { "braveWalletSelectAsset", IDS_BRAVE_WALLET_SELECT_ASSET },
      { "braveWalletSearchAsset", IDS_BRAVE_WALLET_SEARCH_ASSET },
      { "braveWalletSwapFrom", IDS_BRAVE_WALLET_SWAP_FROM },
      { "braveWalletSwapTo", IDS_BRAVE_WALLET_SWAP_TO },
      { "braveWalletSwapEstimate", IDS_BRAVE_WALLET_SWAP_ESTIMATE },
      { "braveWalletSwapMarket", IDS_BRAVE_WALLET_SWAP_MARKET },
      { "braveWalletSwapLimit", IDS_BRAVE_WALLET_SWAP_LIMIT },
      { "braveWalletSwapPriceIn", IDS_BRAVE_WALLET_SWAP_PRICE_IN },
      { "braveWalletBuyTitle", IDS_BRAVE_WALLET_BUY_TITLE },
      { "braveWalletBuyDescription", IDS_BRAVE_WALLET_BUY_DESCRIPTION },
      { "braveWalletBuyWyreButton", IDS_BRAVE_WALLET_BUY_WYRE_BUTTON },
      { "braveWalletBuyFaucetButton", IDS_BRAVE_WALLET_BUY_FAUCET_BUTTON },
      { "braveWalletSignTransactionTitle", IDS_BRAVE_WALLET_SIGN_TRANSACTION_TITLE },
      { "braveWalletSignWarning", IDS_BRAVE_WALLET_SIGN_WARNING },
      { "braveWalletSignWarningTitle", IDS_BRAVE_WALLET_SIGN_WARNING_TITLE },
      { "braveWalletSignTransactionMessageTitle", IDS_BRAVE_WALLET_SIGN_TRANSACTION_MESSAGE_TITLE },
      { "braveWalletSignTransactionButton", IDS_BRAVE_WALLET_SIGN_TRANSACTION_BUTTON },
      { "braveWalletAllowSpendTitle", IDS_BRAVE_WALLET_ALLOW_SPEND_TITLE },
      { "braveWalletAllowSpendDescriptionFirstHalf", IDS_BRAVE_WALLET_ALLOW_SPEND_DESCRIPTION_FIRST_HALF },
      { "braveWalletAllowSpendDescriptionSecondHalf", IDS_BRAVE_WALLET_ALLOW_SPEND_DESCRIPTION_SECOND_HALF },
      { "braveWalletAllowSpendBoxTitle", IDS_BRAVE_WALLET_ALLOW_SPEND_BOX_TITLE },
      { "braveWalletAllowSpendTransactionFee", IDS_BRAVE_WALLET_ALLOW_SPEND_TRANSACTION_FEE },
      { "braveWalletAllowSpendEditButton", IDS_BRAVE_WALLET_ALLOW_SPEND_EDIT_BUTTON },
      { "braveWalletAllowSpendDetailsButton", IDS_BRAVE_WALLET_ALLOW_SPEND_DETAILS_BUTTON },
      { "braveWalletAllowSpendRejectButton", IDS_BRAVE_WALLET_ALLOW_SPEND_REJECT_BUTTON },
      { "braveWalletAllowSpendConfirmButton", IDS_BRAVE_WALLET_ALLOW_SPEND_CONFIRM_BUTTON },
      { "braveWalletAllowAddNetworkTitle", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_TITLE },
      { "braveWalletAllowAddNetworkDescription", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_DESCRIPTION },
      { "braveWalletAllowAddNetworkLearnMoreButton", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_LEARN_MORE_BUTTON },
      { "braveWalletAllowAddNetworkName", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_NAME },
      { "braveWalletAllowAddNetworkUrl", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_URL },
      { "braveWalletAllowAddNetworkDetailsButton", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_DETAILS_BUTTON },
      { "braveWalletAllowAddNetworkApproveButton", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_APPROVE_BUTTON },
      { "braveWalletAllowAddNetworkChainID", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_CHAIN_I_D },
      { "braveWalletAllowAddNetworkCurrencySymbol", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_CURRENCY_SYMBOL },
      { "braveWalletAllowAddNetworkExplorer", IDS_BRAVE_WALLET_ALLOW_ADD_NETWORK_EXPLORER },
      { "braveWalletConfirmTransactionTotal", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_TOTAL },
      { "braveWalletConfirmTransactionGasFee", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_GAS_FEE },
      { "braveWalletConfirmTransactionBid", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_BID },
      { "braveWalletConfirmTransactionAmountGas", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_AMOUNT_GAS },
      { "braveWalletConfirmTransactionNoData", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_NO_DATA },
      { "braveWalletConfirmTransactionNext", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_NEXT },
      { "braveWalletConfirmTransactionFrist", IDS_BRAVE_WALLET_CONFIRM_TRANSACTION_FRIST },
      { "braveWalletConfirmTransactions", IDS_BRAVE_WALLET_CONFIRM_TRANSACTIONS },
      { "braveWalletPanelTitle", IDS_BRAVE_WALLET_PANEL_TITLE },
      { "braveWalletPanelConnected", IDS_BRAVE_WALLET_PANEL_CONNECTED },
      { "braveWalletPanelNotConnected", IDS_BRAVE_WALLET_PANEL_NOT_CONNECTED },
      { "braveWalletTransactionDetailBoxFunction", IDS_BRAVE_WALLET_TRANSACTION_DETAIL_BOX_FUNCTION },
      { "braveWalletTransactionDetailBoxHex", IDS_BRAVE_WALLET_TRANSACTION_DETAIL_BOX_HEX },
      { "braveWalletTransactionDetailBoxBytes", IDS_BRAVE_WALLET_TRANSACTION_DETAIL_BOX_BYTES },
      { "braveWalletConnectWithSiteTitle", IDS_BRAVE_WALLET_CONNECT_WITH_SITE_TITLE },
      { "braveWalletConnectWithSiteDescription1", IDS_BRAVE_WALLET_CONNECT_WITH_SITE_DESCRIPTION1 },
      { "braveWalletConnectWithSiteDescription2", IDS_BRAVE_WALLET_CONNECT_WITH_SITE_DESCRIPTION2 },
      { "braveWalletConnectWithSiteNext", IDS_BRAVE_WALLET_CONNECT_WITH_SITE_NEXT },
      { "braveWalletConnectWithSiteHeaderTitle", IDS_BRAVE_WALLET_CONNECT_WITH_SITE_HEADER_TITLE },
      { "braveWalletImportFromExternalNewPassword", IDS_BRAVE_WALLET_IMPORT_FROM_EXTERNAL_NEW_PASSWORD },
      { "braveWalletImportFromExternalCreatePassword", IDS_BRAVE_WALLET_IMPORT_FROM_EXTERNAL_CREATE_PASSWORD },
      { "braveWalletImportFromExternalPasswordCheck", IDS_BRAVE_WALLET_IMPORT_FROM_EXTERNAL_PASSWORD_CHECK },
      { "braveWalletImportMetaMaskTitle", IDS_BRAVE_WALLET_IMPORT_META_MASK_TITLE },
      { "braveWalletImportMetaMaskDescription", IDS_BRAVE_WALLET_IMPORT_META_MASK_DESCRIPTION },
      { "braveWalletImportMetaMaskInput", IDS_BRAVE_WALLET_IMPORT_META_MASK_INPUT },
      { "braveWalletImportBraveLegacyTitle", IDS_BRAVE_WALLET_IMPORT_BRAVE_LEGACY_TITLE },
      { "braveWalletImportBraveLegacyDescription", IDS_BRAVE_WALLET_IMPORT_BRAVE_LEGACY_DESCRIPTION },
      { "braveWalletImportBraveLegacyInput", IDS_BRAVE_WALLET_IMPORT_BRAVE_LEGACY_INPUT },
      { "braveWalletImportBraveLegacyAltButton", IDS_BRAVE_WALLET_IMPORT_BRAVE_LEGACY_ALT_BUTTON },
      { "braveWalletConnectHardwarePanelConnected", IDS_BRAVE_WALLET_CONNECT_HARDWARE_PANEL_CONNECTED },
      { "braveWalletConnectHardwarePanelDisconnected", IDS_BRAVE_WALLET_CONNECT_HARDWARE_PANEL_DISCONNECTED },
      { "braveWalletConnectHardwarePanelInstructions", IDS_BRAVE_WALLET_CONNECT_HARDWARE_PANEL_INSTRUCTIONS },
      { "braveWalletConnectHardwarePanelConnect", IDS_BRAVE_WALLET_CONNECT_HARDWARE_PANEL_CONNECT },
      { "braveWalletConnectHardwarePanelConfirmation", IDS_BRAVE_WALLET_CONNECT_HARDWARE_PANEL_CONFIRMATION },
      { "braveWalletTransactionSent", IDS_BRAVE_WALLET_TRANSACTION_SENT },
      { "braveWalletTransactionReceived", IDS_BRAVE_WALLET_TRANSACTION_RECEIVED },
      { "braveWalletTransactionExplorerMissing", IDS_BRAVE_WALLET_TRANSACTION_EXPLORER_MISSING },
      { "braveWalletTransactionExplorer", IDS_BRAVE_WALLET_TRANSACTION_EXPLORER },
      { "braveWalletTransactionPlaceholder", IDS_BRAVE_WALLET_TRANSACTION_PLACEHOLDER },
      { "braveWalletEditGasTitle1", IDS_BRAVE_WALLET_EDIT_GAS_TITLE1 },
      { "braveWalletEditGasTitle2", IDS_BRAVE_WALLET_EDIT_GAS_TITLE2 },
      { "braveWalletEditGasDescription", IDS_BRAVE_WALLET_EDIT_GAS_DESCRIPTION },
      { "braveWalletEditGasLow", IDS_BRAVE_WALLET_EDIT_GAS_LOW },
      { "braveWalletEditGasOptimal", IDS_BRAVE_WALLET_EDIT_GAS_OPTIMAL },
      { "braveWalletEditGasHigh", IDS_BRAVE_WALLET_EDIT_GAS_HIGH },
      { "braveWalletEditGasLimit", IDS_BRAVE_WALLET_EDIT_GAS_LIMIT },
      { "braveWalletEditGasPerGasPrice", IDS_BRAVE_WALLET_EDIT_GAS_PER_GAS_PRICE },
      { "braveWalletEditGasPerTipLimit", IDS_BRAVE_WALLET_EDIT_GAS_PER_TIP_LIMIT },
      { "braveWalletEditGasPerPriceLimit", IDS_BRAVE_WALLET_EDIT_GAS_PER_PRICE_LIMIT },
      { "braveWalletEditGasMaxFee", IDS_BRAVE_WALLET_EDIT_GAS_MAX_FEE },
      { "braveWalletEditGasMaximumFee", IDS_BRAVE_WALLET_EDIT_GAS_MAXIMUM_FEE },
      { "braveWalletEditGasBaseFee", IDS_BRAVE_WALLET_EDIT_GAS_BASE_FEE },
      { "braveWalletEditGasGwei", IDS_BRAVE_WALLET_EDIT_GAS_GWEI },
      { "braveWalletEditGasSetCustom", IDS_BRAVE_WALLET_EDIT_GAS_SET_CUSTOM },
      { "braveWalletEditGasSetSuggested", IDS_BRAVE_WALLET_EDIT_GAS_SET_SUGGESTED },
  };
  source->AddLocalizedStrings(kStrings);
  NavigationBarDataProvider::Initialize(source);
  webui::SetupWebUIDataSource(
      source,
      base::make_span(kBraveWalletPageGenerated, kBraveWalletPageGeneratedSize),
      IDR_WALLET_PAGE_HTML);
  auto* profile = Profile::FromWebUI(web_ui);
  content::WebUIDataSource::Add(profile, source);
  brave_wallet::AddERCTokenImageSource(profile);
}

WalletPageUI::~WalletPageUI() = default;

WEB_UI_CONTROLLER_TYPE_IMPL(WalletPageUI)

void WalletPageUI::BindInterface(
    mojo::PendingReceiver<brave_wallet::mojom::PageHandlerFactory> receiver) {
  page_factory_receiver_.reset();
  page_factory_receiver_.Bind(std::move(receiver));
}

void WalletPageUI::CreatePageHandler(
    mojo::PendingRemote<brave_wallet::mojom::Page> page,
    mojo::PendingReceiver<brave_wallet::mojom::PageHandler> page_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::WalletHandler> wallet_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::EthJsonRpcController>
        eth_json_rpc_controller_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::SwapController>
        swap_controller_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::AssetRatioController>
        asset_ratio_controller_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::KeyringController>
        keyring_controller_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::ERCTokenRegistry>
        erc_token_registry_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::EthTxController>
        eth_tx_controller_receiver,
    mojo::PendingReceiver<brave_wallet::mojom::BraveWalletService>
        brave_wallet_service_receiver) {
  DCHECK(page);
  auto* profile = Profile::FromWebUI(web_ui());
  DCHECK(profile);

  page_handler_ =
      std::make_unique<WalletPageHandler>(std::move(page_receiver), profile);
  wallet_handler_ =
      std::make_unique<WalletHandler>(std::move(wallet_receiver), profile);

  auto* eth_json_rpc_controller =
      brave_wallet::RpcControllerFactory::GetControllerForContext(profile);
  if (eth_json_rpc_controller) {
    eth_json_rpc_controller->Bind(std::move(eth_json_rpc_controller_receiver));
  }

  auto* swap_controller =
      brave_wallet::SwapControllerFactory::GetControllerForContext(profile);
  if (swap_controller) {
    swap_controller->Bind(std::move(swap_controller_receiver));
  }

  auto* asset_ratio_controller =
      brave_wallet::AssetRatioControllerFactory::GetControllerForContext(
          profile);
  if (asset_ratio_controller) {
    asset_ratio_controller->Bind(std::move(asset_ratio_controller_receiver));
  }

  auto* keyring_controller =
      brave_wallet::KeyringControllerFactory::GetControllerForContext(profile);
  if (keyring_controller) {
    keyring_controller->Bind(std::move(keyring_controller_receiver));
  }

  auto* erc_token_registry = brave_wallet::ERCTokenRegistry::GetInstance();
  if (erc_token_registry) {
    erc_token_registry->Bind(std::move(erc_token_registry_receiver));
  }

  auto* eth_tx_controller =
      brave_wallet::EthTxControllerFactory::GetControllerForContext(profile);
  if (eth_tx_controller) {
    eth_tx_controller->Bind(std::move(eth_tx_controller_receiver));
  }

  auto* brave_wallet_service =
      brave_wallet::BraveWalletServiceFactory::GetServiceForContext(profile);
  if (brave_wallet_service) {
    brave_wallet_service->Bind(std::move(brave_wallet_service_receiver));
  }
}
