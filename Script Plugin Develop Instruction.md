

## Script Plugin Develop Instruction

### Messages

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_LOGA</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int level</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[in] char * msg</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">save msg to log file</td>
		</tr>
		<tr>
			<td width="10%" rowspan="3" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White">level = 0 means log msg as information</td>
		</tr>
		<tr>
			<td width="90%" colspan="3" style="background:White">level = 1 means log msg as warnning</td>
		</tr>
		<tr>
			<td width="90%" colspan="3" style="background:White">level = 2 means log msg as error</td>
		</tr>		
	</tbody>
</table>
<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_LOGW</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int level</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[in] wchar_t * msg</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">same as CWM_LOGA</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White">same as CVM_LOGA</td>
		</tr>
	</tbody>
</table>


<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_FETCH_DOCUMENT</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int lengthOfDoc</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[out] wchar_t * doc</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">receives the full document from Npp. User is responsible to allocate a buffer with an enough size (lengthOfDoc)</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White">when doc is NULL, NPP will return the require size minus one in wParam(require pointer to a int)</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_UPDATE_DOCUMENT</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int lengthOfDoc</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[out] wchar_t * doc</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">update doc to Npp</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>
<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_FETCH_SELECTION</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int lengthOfSel</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[out] wchar_t * sel</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">receives the selection from Npp. User is responsible to allocate a buffer with an enough (lengthOfDoc) size.</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White">when sel is NULL, Npp will return the require size minus one in wParam(require pointer to a int)</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_UPDATE_SELECTION</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int lengthOfSel</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[out] wchar_t * sel</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">replace the selction of Npp with sel</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>


<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_GET_DOCUMENT</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int lengthOfDoc</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[out] wchar_t * doc</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">get document from CDocument. User is responsible to allocate a buffer with an enough size(lengthOfDoc).</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White">when doc is NULL, NPP will return the require size minus one in wParam(require pointer to a int).</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_SET_DOCUMENT</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int lengthOfDoc</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[out] wchar_t * doc</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">set doc to CDocument</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_PARSE</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>[in] int indexType</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>[in] wchar_t * indexStr</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">parse the document in CDocument.</td>
		</tr>
		<tr>
			<td width="10%" rowspan="3" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White">indexType = 0 menas parse using [content] item in ContentsViewerPlugin.ini</td>
		</tr>
		<tr>
			<td width="90%" colspan="3" style="background:White">indexType = 1 means index head line</td>
		</tr>
		<tr>
			<td width="90%" colspan="3" style="background:White">indexType = 2 means index with indexStr</td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_DEFAULT_REPLACE</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>NULL</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>NULL</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">replace the document in theDocument Module with stdRep.jb</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_NUMBER_CONTENTS</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>NULL</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>NULL</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">convert Chinese numerals in contens to Arabic numerals in theDocument Module</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_TRIM_CONTENTS</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>NULL</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>NULL</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">omit the leading and trailing whitespace of contens in theDocument Module</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>

<table border="1" width="100%">
	<tbody>
		<tr>
			<td width="50%" rowspan="2" colspan="2" style="background:LightYellow;">CVM_MERGE_PARAGRAPHS</td>
			<td width="10%" style="background:khaki">wParam:</td>
			<td>NULL</td></tr>
		<tr>
			<td width="10%" style="background:khaki">lParam:</td>
			<td>NULL</td>
		</tr>
		<tr>
			<td colspan="4" style="background:#E8F8E8;">merge invalid paragraphs in theDocument Module</td>
		</tr>
		<tr>
			<td width="10%" style="background:#dae7da;">Note:</td>
			<td width="90%" colspan="3" style="background:White"></td>
		</tr>
	</tbody>
</table>
### Function Path

#### Parse

1. IDB_PARSE => Parse(m_stIndexType, m_wIndex.c_str())
2. IDM_HEAD_INDEX_CONTENTS => HeadIndex() => Parse(m_stIndexType, m_wIndex.c_str())
3. IDM_SPECIFIC_INDEX_CONTENTS => SpecificIndex() => Parse(m_stIndexType, m_wIndex.c_str())
4. CVM_PARSE => Check menu item => Parse(m_stIndexType, m_wIndex.c_str())



