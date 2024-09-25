import { ComponentFixture, TestBed } from "@angular/core/testing";
import { SidebarButtonShipsComponent } from "./sidebar-button-ships.component";

describe("SidebarButtonShipsComponent", () => {
  let component: SidebarButtonShipsComponent;
  let fixture: ComponentFixture<SidebarButtonShipsComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      imports: [SidebarButtonShipsComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(SidebarButtonShipsComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it("should create", () => {
    expect(component).toBeTruthy();
  });
});
