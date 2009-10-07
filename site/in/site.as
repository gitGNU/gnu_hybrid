autogen definitions sitetool;

common = {
	layout = "./layout.sxml";
};

vars = {
        sitename          = "Hybrid";
        package_url       = "http://savannah.nongnu.org/projects/hybrid";
        package_bugreport = "hybrid-generic@nongnu.org";
        ohloh_badge_url   = "http://www.ohloh.net/p/26028/widgets/project_partner_badge.js";
};

include "contents.as"
include "pages.as"
include "map.as"
include "files.as"
