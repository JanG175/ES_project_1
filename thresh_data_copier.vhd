library IEEE;
use IEEE.STD_LOGIC_1164.ALL;
use IEEE.NUMERIC_STD.ALL;

entity data_copier is
    port (
        clk         :   in  STD_LOGIC;
        
        bram0_din   :   in  STD_LOGIC_VECTOR(31 downto 0);
        bram0_wen   :   out STD_LOGIC_VECTOR(3 downto 0);
        bram0_addr  :   out STD_LOGIC_VECTOR(31 downto 0);

        bram1_dout  :   out STD_LOGIC_VECTOR(31 downto 0);
        bram1_wen   :   out STD_LOGIC_VECTOR(3 downto 0);
        bram1_addr  :   out STD_LOGIC_VECTOR(31 downto 0);

        bram_en     :   out STD_LOGIC;
        bram_rst    :   out STD_LOGIC
    );
end data_copier;

architecture Behavioral of data_copier is
    signal cnt      :   UNSIGNED(31 downto 0)   :=  (others => '0');
    signal thresh_dout : UNSIGNED(31 downto 0) := (others => '0');

begin

    cnt_process: process(clk)
    begin
        if (rising_edge(clk)) then
            cnt     <=  cnt + "1";
        end if;
        
        for i in 0 to 3 loop
            if (to_integer(UNSIGNED(bram0_din((i+1)*8-1 downto i*8))) >= 100) then
                thresh_dout((i+1)*8-1 downto i*8) <= (others => '1');
            else
                thresh_dout((i+1)*8-1 downto i*8) <= (others => '0');
            end if;
        end loop;
    end process;

    bram_en     <=  '1';
    bram_rst    <=  '0';

    bram0_wen   <=  (others => '0');
    bram1_wen   <=  (others => STD_LOGIC(cnt(1)));

    bram1_dout  <=  STD_LOGIC_VECTOR(thresh_dout);

    bram0_addr(31 downto 14)    <=  (30 => '1', others => '0');
    bram0_addr(13 downto 2)     <=  STD_LOGIC_VECTOR(cnt(13 downto 2));
    bram0_addr(1 downto 0)      <=  "00";

    bram1_addr(31 downto 13)    <=  (30 => '1', others => '0');
    bram1_addr(13 downto 2)     <=  STD_LOGIC_VECTOR(cnt(13 downto 2));
    bram1_addr(1 downto 0)      <=  "00";

end Behavioral;