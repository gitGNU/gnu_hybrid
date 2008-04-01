contents = {
	content = {
		id      = "p1_body";
		source  = "index.scm";
		filters = {
			filter[0] = {
				source      = "sxml";
				destination = "sxml";
				parameters  = "";
			};
		};
	};

        content = {
                id     = "footer";
                source = "./footer.sxml";
                filters = {
                        filter[0] = {
                                source      = "sxml";
                                destination = "sxml";
                                parameters  = "";
                        };
                };
        };

        content = {
                id     = "header";
                source = "./header.sxml";
                filters = {
                        filter[0] = {
                                source      = "sxml";
                                destination = "sxml";
                                parameters  = "";
                        };
                };
        };
};
