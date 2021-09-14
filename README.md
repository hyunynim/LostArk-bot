# Discord bot for LostArk

# Command List(start with '!')
- 하위
- 강화 [확률]
- 주사위
- 원정대등록 [캐릭터 명]
  - 원정대 등록 시, 원정대에 포함되어 있는 캐릭터 중, 1325 레벨 이상의 모든 캐릭터 등록
- 원정대 [캐릭터 명] [주간|일일]
  - 해당 캐릭터의 주간/일일 컨텐츠 목록 확인
- 캐릭터추가 [캐릭터 명]
- 마리 [크리스탈 가격]
- 항아리
  - 성공 확률 10%
- 일정등록 [이름] [난이도] [요일] [시간]
  - [이름], [난이도], [요일], [시간]에 공백 포함될 수 없음
  - 모든 일정 등록 후, 일정확정 명령어를 사용하면 투표 시작
- 일정확정
- 일정확인
  - 일정이 확정된 경우 투표 가능한 번호 출력
- 일정투표 [캐릭터명] [번호]
  - 등록된 일정 중 번호에 맞는 레이드에 해당 캐릭터 등록

# Todo
- Edit weekly, daily data form 
  - Use index for DB
- Scheduling for commands
- FILE I/O for schedule
- Refactoring
  - Function that get information from web
  - Commands(onMessage)
  - Util function(for string)
- Expedition scheduler

# 2021-09-14
 - Add commands
   - 일정등록, 일정확인, 일정확정, 일정투표
 - Edit checking start string 

# 2021-09-09
- Add commands
  - 원정대, 항아리
- Add expedition managing
- Edit commands
  - 마리
    - 개당 가격 추가
  - 원정대등록
    - Add data to expedition list

# 2021-09-08
- Command List(starts with '!')
  - 하위
  - 강화 [확률]
  - 주사위
  - 원정대등록 [캐릭터 명]
  - 캐릭터추가 [캐릭터 명]
  - 마리 [크리스탈 가격]
- Expedition, character class
