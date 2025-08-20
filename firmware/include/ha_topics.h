#include <utils.h>

// Core identifiers
static inline String haNodeId() { return "board-" + toLower(getMacSuffix()); }
static inline String haObjectId() { return haNodeId() + "-light"; }

// Light (main controllable entity)
static inline String haConfigTopic() { return "homeassistant/light/" + haNodeId() + "/config"; }
static inline String haCmdTopic() { return "console/" + haNodeId() + "/ha/set"; }
static inline String haStateTopic() { return "console/" + haNodeId() + "/ha/state"; }
static inline String haAvailTopic() { return "console/" + haNodeId() + "/status"; }

// Number (Offset)
static inline String haNumberOffsetConfigTopic() { return "homeassistant/number/" + haNodeId() + "/offset/config"; }
static inline String haOffsetCmdTopic() { return "console/" + haNodeId() + "/offset/set"; }
static inline String haOffsetStateTopic() { return "console/" + haNodeId() + "/offset/state"; }

// Sensors (read-only)
static inline String haSensorBaselineConfigTopic() { return "homeassistant/sensor/" + haNodeId() + "/threshold/config"; }
static inline String haBaseStateTopic() { return "console/" + haNodeId() + "/threshold/state"; }

static inline String haSensorOnConfigTopic() { return "homeassistant/sensor/" + haNodeId() + "/th_on/config"; }
static inline String haThOnStateTopic() { return "console/" + haNodeId() + "/th_on/state"; }

static inline String haSensorOffConfigTopic() { return "homeassistant/sensor/" + haNodeId() + "/th_off/config"; }
static inline String haThOffStateTopic() { return "console/" + haNodeId() + "/th_off/state"; }

// Buttons (stateless actions)
static inline String haIdentifyConfigTopic() { return "homeassistant/button/" + haNodeId() + "/identify/config"; }
static inline String haIdentifyCmdTopic() { return "console/" + haNodeId() + "/identify"; }

static inline String haRebootConfigTopic() { return "homeassistant/button/" + haNodeId() + "/reboot/config"; }
static inline String haRebootCmdTopic() { return "console/" + haNodeId() + "/reboot"; }

static inline String haCalibrateConfigTopic() { return "homeassistant/button/" + haNodeId() + "/calibrate/config"; }
static inline String haCalibrateCmdTopic() { return "console/" + haNodeId() + "/calibrate"; }