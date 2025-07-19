function copy_rttr(platform, outputdir)
	local rttr_path

	if platform == "windows" then
		rttr_path = path.join("%{wks.location}/out/bin/", outputdir, "rttr/rttr.dll")
	elseif platform == "linux" then
		rttr_path = path.join("%{wks.location}/out/bin/", outputdir, "rttr/librttr.so")
	else
		print("Warning: RTTR copy not supported on " .. platform)
		return
	end

	postbuildcommands {
		"{COPY} " .. rttr_path .. " " .. "%{cfg.targetdir}"
	}
end