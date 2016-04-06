<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="9008000">
	<Item Name="My Computer" Type="My Computer">
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="TelemGUI_v1_3.vi" Type="VI" URL="../TelemGUI_v1_3.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Dynamic To Waveform Array.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/Dynamic To Waveform Array.vi"/>
				<Item Name="exnFormulaBlock how many pts in the wave.vi" Type="VI" URL="/&lt;vilib&gt;/express/express arith-compare/Formula/Block/exnFormulaBlock how many pts in the wave.vi"/>
				<Item Name="ex_Modify Signal Name.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/ex_Modify Signal Name.vi"/>
				<Item Name="Waveform Array To Dynamic.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/Waveform Array To Dynamic.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="General Error Handler CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler CORE.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="Simple Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Simple Error Handler.vi"/>
				<Item Name="VISA Configure Serial Port (Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Instr).vi"/>
				<Item Name="VISA Configure Serial Port (Serial Instr).vi" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port (Serial Instr).vi"/>
				<Item Name="VISA Configure Serial Port" Type="VI" URL="/&lt;vilib&gt;/Instr/_visa.llb/VISA Configure Serial Port"/>
			</Item>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="Telemetry GUI executable" Type="EXE">
				<Property Name="App_applicationGUID" Type="Str">{2283EA6D-746D-4744-8C11-A167A3E8F7D1}</Property>
				<Property Name="App_applicationName" Type="Str">MSCP_TelemGUI_v1_3.exe</Property>
				<Property Name="App_companyName" Type="Str">McMaster University</Property>
				<Property Name="App_fileDescription" Type="Str">Telemetry GUI executable</Property>
				<Property Name="App_fileVersion.build" Type="Int">1002</Property>
				<Property Name="App_fileVersion.major" Type="Int">1</Property>
				<Property Name="App_fileVersion.minor" Type="Int">3</Property>
				<Property Name="App_fileVersion.patch" Type="Int">2</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{D547BB41-9B89-44A8-9D13-D865C24BDE68}</Property>
				<Property Name="App_INI_GUID" Type="Str">{1AB77C1A-27E4-4124-B4E0-B931602702B6}</Property>
				<Property Name="App_internalName" Type="Str">Telemetry GUI executable</Property>
				<Property Name="App_legalCopyright" Type="Str">Copyright © 2014 McMaster Solar Car Project</Property>
				<Property Name="App_productName" Type="Str">McMaster Solar Car Project Telemetry GUI</Property>
				<Property Name="Bld_buildSpecName" Type="Str">Telemetry GUI executable</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Destination[0].destName" Type="Str">MSCP_TelemGUI_v1_3.exe</Property>
				<Property Name="Destination[0].path" Type="Path">../builds/Telemetry GUI executable/NI_AB_PROJECTNAME/MSCP_TelemGUI_v1_3.exe</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">../builds/Telemetry GUI executable/NI_AB_PROJECTNAME/data</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Source[0].itemID" Type="Str">{25E4CFC3-6B2A-41AC-8594-71BF9D54A48A}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/TelemGUI_v1_3.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">2</Property>
			</Item>
			<Item Name="MSCP Telemetry GUI Installer" Type="Installer">
				<Property Name="arpCompany" Type="Str">McMaster Solar Car Project</Property>
				<Property Name="arpURL" Type="Str">http://www.macsolarcar.com</Property>
				<Property Name="BldInfo.Count" Type="Int">1</Property>
				<Property Name="BldInfo[0].Dir" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="BldInfo[0].Tag" Type="Ref">/My Computer/Build Specifications/Telemetry GUI executable</Property>
				<Property Name="BuildLabel" Type="Str">MSCP Telemetry GUI Installer</Property>
				<Property Name="BuildLocation" Type="Path">../../../../../../Desktop/Telemetry Installer</Property>
				<Property Name="DirInfo.Count" Type="Int">1</Property>
				<Property Name="DirInfo.DefaultDir" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="DirInfo[0].DirName" Type="Str">TelemGUI_v1_3</Property>
				<Property Name="DirInfo[0].DirTag" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="DirInfo[0].ParentTag" Type="Str">{3912416A-D2E5-411B-AFEE-B63654D690C0}</Property>
				<Property Name="DistID" Type="Str">{F3879760-437C-4DE8-A4F6-E9EC77A17FDF}</Property>
				<Property Name="DistParts.Count" Type="Int">2</Property>
				<Property Name="DistPartsInfo[0].FlavorID" Type="Str">DefaultFull</Property>
				<Property Name="DistPartsInfo[0].ProductID" Type="Str">{7312C1D1-70C8-400E-9CAF-AF1DD2763269}</Property>
				<Property Name="DistPartsInfo[0].ProductName" Type="Str">NI LabVIEW Run-Time Engine 2009 SP1</Property>
				<Property Name="DistPartsInfo[0].UpgradeCode" Type="Str">{1DA01FF3-1E36-4A14-802B-D195819E159B}</Property>
				<Property Name="DistPartsInfo[1].FlavorID" Type="Str">_deployment_</Property>
				<Property Name="DistPartsInfo[1].ProductID" Type="Str">{BEB32B6A-2984-40F3-B24E-DB29311A1DD2}</Property>
				<Property Name="DistPartsInfo[1].ProductName" Type="Str">NI-VISA Runtime 4.6.2</Property>
				<Property Name="DistPartsInfo[1].UpgradeCode" Type="Str">{8627993A-3F66-483C-A562-0D3BA3F267B1}</Property>
				<Property Name="FileInfo.Count" Type="Int">3</Property>
				<Property Name="FileInfo[0].DirTag" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="FileInfo[0].FileName" Type="Str">MSCP_TelemGUI_v1_3.exe</Property>
				<Property Name="FileInfo[0].FileTag" Type="Str">{2283EA6D-746D-4744-8C11-A167A3E8F7D1}</Property>
				<Property Name="FileInfo[0].Type" Type="Int">3</Property>
				<Property Name="FileInfo[0].TypeID" Type="Ref">/My Computer/Build Specifications/Telemetry GUI executable</Property>
				<Property Name="FileInfo[1].DirTag" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="FileInfo[1].FileName" Type="Str">MSCP_TelemGUI_v1_3.aliases</Property>
				<Property Name="FileInfo[1].FileTag" Type="Str">{D547BB41-9B89-44A8-9D13-D865C24BDE68}</Property>
				<Property Name="FileInfo[1].Type" Type="Int">3</Property>
				<Property Name="FileInfo[1].TypeID" Type="Ref">/My Computer/Build Specifications/Telemetry GUI executable</Property>
				<Property Name="FileInfo[2].DirTag" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="FileInfo[2].FileName" Type="Str">MSCP_TelemGUI_v1_3.ini</Property>
				<Property Name="FileInfo[2].FileTag" Type="Str">{1AB77C1A-27E4-4124-B4E0-B931602702B6}</Property>
				<Property Name="FileInfo[2].Type" Type="Int">3</Property>
				<Property Name="FileInfo[2].TypeID" Type="Ref">/My Computer/Build Specifications/Telemetry GUI executable</Property>
				<Property Name="InstSpecVersion" Type="Str">9018011</Property>
				<Property Name="LicenseFile" Type="Ref"></Property>
				<Property Name="OSCheck" Type="Int">0</Property>
				<Property Name="OSCheck_Vista" Type="Bool">false</Property>
				<Property Name="ProductName" Type="Str">MSCP Telemetry GUI</Property>
				<Property Name="ProductVersion" Type="Str">1.3.3</Property>
				<Property Name="ReadmeFile" Type="Ref"></Property>
				<Property Name="ShortcutInfo.Count" Type="Int">1</Property>
				<Property Name="ShortcutInfo[0].DirTag" Type="Str">{B9E310F1-839C-48B7-8CAE-33000780C26E}</Property>
				<Property Name="ShortcutInfo[0].FileTag" Type="Str">{2283EA6D-746D-4744-8C11-A167A3E8F7D1}</Property>
				<Property Name="ShortcutInfo[0].FileTagDir" Type="Str">{A953DB1B-161F-4A7D-B105-A84E31CE11BC}</Property>
				<Property Name="ShortcutInfo[0].Name" Type="Str">Telemetry GUI</Property>
				<Property Name="ShortcutInfo[0].SubDir" Type="Str">McMaster Solar Car Project</Property>
				<Property Name="UpgradeCode" Type="Str">{EE05D34B-F454-43F0-A228-73F298664883}</Property>
				<Property Name="WindowMessage" Type="Str">This package installs the telemetry GUI for the McMaster Solar Car Project along with the LabVIEW Runtime Environment. If this computer already has LabVIEW 2009 installed, you may use the Telemetry GUI executable package instead.</Property>
				<Property Name="WindowTitle" Type="Str">McMaster Solar Car Project: Telemetry GUI</Property>
			</Item>
		</Item>
	</Item>
</Project>
