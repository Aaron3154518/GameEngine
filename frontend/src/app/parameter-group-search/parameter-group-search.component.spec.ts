import { ComponentFixture, TestBed } from '@angular/core/testing';

import { ParameterGroupSearchComponent } from './parameter-group-search.component';

describe('ParameterGroupSearchComponent', () => {
  let component: ParameterGroupSearchComponent;
  let fixture: ComponentFixture<ParameterGroupSearchComponent>;

  beforeEach(async () => {
    await TestBed.configureTestingModule({
      declarations: [ParameterGroupSearchComponent],
    }).compileComponents();

    fixture = TestBed.createComponent(ParameterGroupSearchComponent);
    component = fixture.componentInstance;
    fixture.detectChanges();
  });

  it('should create', () => {
    expect(component).toBeTruthy();
  });
});
